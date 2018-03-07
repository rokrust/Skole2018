#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>
#include <limits>
#include <array>
/********************************** Population **********************************/
Population::Population(int population_size, int archive_size): archive_size(archive_size) {
	population.reserve(population_size);

	for (int i = 0; i < population_size; i++) {
		Genotype genotype;
		MST mst;
		mst.genotype_generator(genotype);
		population.push_back(genotype);
	}
}
/********************************************************************************/

/*********************************** Phenotype **********************************/
void Phenotype::add_ingoing_to_active_edge(int row, int col) {
	Index dummy = { -1, -1 };
	std::array<Index, 4> neighbor = { dummy, dummy, dummy, dummy };

	image.get_neighbors(row, col, neighbor);

	for (int pixel = 0; pixel < 4; pixel++) {
		Index current_neighbor = neighbor[pixel];
		if (current_neighbor.is_none()) {
			continue;
		}
		
		GRAPH_EDGE_DIR dir = (*genotype)[current_neighbor.row][current_neighbor.col]; //garbage
		Index next_neighbor = image.next_index(current_neighbor.row, current_neighbor.col, dir);
		if (next_neighbor.is_none()) {
			continue;
		}

		//Add neighbor if it points to the segment
		if (!is_part_of_segment[current_neighbor.row][current_neighbor.col] &&
			is_part_of_segment[next_neighbor.row][next_neighbor.col]) {
			
			is_part_of_segment[current_neighbor.row][current_neighbor.col] = true;
			new_active_edge.push_back(current_neighbor);
		}
	}

}

void Phenotype::add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir) {
	if (dir != NONE) {
		Index next_pixel = image.next_index(row, col, dir);
		if (next_pixel.is_none()) {
			return;
		}

		if (!is_part_of_segment[next_pixel.row][next_pixel.col]) {
			is_part_of_segment[next_pixel.row][next_pixel.col] = true;
			new_active_edge.push_back(next_pixel);
		}

	}
}

void Phenotype::build_segment_from_pixel(int row, int col) {
	active_edge.push_back({ row, col });
	is_part_of_segment[row][col] = true;
	Segment current_segment;
	
	static int segment_id = 0;
	segment_id++;

	// As long as a pixel was added to the segment
	while (active_edge.size() != 0) {
		new_active_edge.clear();
		for (int i = active_edge.size() - 1; i >= 0; i--) {
			Index current_pixel = active_edge[i];
			
			current_segment.add_pixel(current_pixel.row, current_pixel.col);
			active_edge.pop_back();
			belongs_to_segment[current_pixel.row][current_pixel.col] = segment_id;

			//Add all neighbors pointing towards the segment
			add_ingoing_to_active_edge(current_pixel.row, current_pixel.col);	//Kalles mange ganger på samme pixel?

			//Add all neighbors being pointed to by the segment
			GRAPH_EDGE_DIR dir = (*genotype)[current_pixel.row][current_pixel.col];
			add_outgoing_to_active_edge(current_pixel.row, current_pixel.col, dir);

		}

		//All members of the old edge handled. Go on to new edge
		active_edge = new_active_edge;
	}

	segment.push_back(current_segment);
}

void Phenotype::build_segments() {
	std::vector<Segment> segments;
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			//Found a pixel that is not a part of a segment
			if (!is_part_of_segment[row][col]) {
				build_segment_from_pixel(row, col);
			}

		}
	}
}

Phenotype::Phenotype() {
	is_part_of_segment = new bool*[IMAGE_HEIGHT];
	belongs_to_segment = new int*[IMAGE_HEIGHT];
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		is_part_of_segment[row] = new bool[IMAGE_WIDTH];
		belongs_to_segment[row] = new int[IMAGE_WIDTH];

		for (int col = 0; col < IMAGE_WIDTH; col++) {
			is_part_of_segment[row][col] = false;
			belongs_to_segment[row][col] = -1;
		}
	}
}

Phenotype::~Phenotype() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] is_part_of_segment[row];
		delete[] belongs_to_segment[row];
	}

}
Phenotype::Phenotype(const Genotype& genotype): Phenotype() {
	this->genotype = &genotype;
	build_segments();
}

double Phenotype::calculate_overall_deviation() {
	double overall_deviation = 0;

	//For each segment
	for (std::vector<Segment>::iterator current_segment = segment.begin();
		current_segment != segment.end();
		current_segment++) {

		double current_deviation = 0;
		Pixel current_centroid = current_segment->calculate_centroid();

		//For each pixel of segment
		for (std::vector<Index>::iterator current_index = current_segment->begin();
			current_index != current_segment->end();
			current_index++) {
			Pixel current_pixel = image[current_index->row][current_index->col];
			current_deviation += current_pixel.color_distance(current_centroid);
		}

		overall_deviation += current_deviation;

	}

	return overall_deviation;
}

double Phenotype::calculate_edge_value() {
	double edge_value = 0.0;

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			Index dummy = { -1, -1 };
			std::array<Index, 4> neighbor = {dummy, dummy, dummy, dummy};

			image.get_neighbors(row, col, neighbor);
			Index current_pixel = { row, col };
			int current_pixel_id = belongs_to_segment[current_pixel.row][current_pixel.col];

			for (int i = 0; i < 4; i++) {
				Index current_neighbor = neighbor[i];
				if (current_neighbor.is_none()) {
					continue;
				}
				int current_neighbor_id = belongs_to_segment[current_neighbor.row][current_neighbor.col];

				if (current_neighbor_id != current_pixel_id) {
					Pixel p1 = image[current_pixel.row][current_pixel.col];
					Pixel p2 = image[current_neighbor.row][current_neighbor.col];
					edge_value += p1.color_distance(p2);
				}
			}
		}
	}

	return edge_value;
}

/********************************************************************************/


/************************************* MST **************************************/
std::vector<std::array<Index, 4>> MST::find_outline() {
	std::vector<std::array<Index, 4>> neighbor(mst_set.size());

	for (int i = 0; i < mst_set.size(); i++) {
		Index current_index = mst_set[i];

		image.get_neighbors(current_index.row, current_index.col, neighbor[i]);
	}

	return neighbor;
}

void MST::update_costs(std::vector<std::array<Index, 4>> &neighbor) {
	for (int current_index = 0; current_index < mst_set.size(); current_index++) {
		for (int current_dir = 0; current_dir < 4; current_dir++) {
			Index current_neighbor = neighbor[current_index][current_dir];
			Index current_pixel = mst_set[current_index];

			if (current_neighbor.is_none() || visited[current_neighbor.row][current_neighbor.col]) {
				continue;
			}

			Pixel current_neighbor_pixel_value = image[current_neighbor.row][current_neighbor.col];
			Pixel current_pixel_value = image[current_pixel.row][current_pixel.col];
			double current_best_cost = edge_cost[current_neighbor.row][current_neighbor.col];
			double current_cost = current_pixel_value.color_distance(current_neighbor_pixel_value);
			
			if (current_cost < current_best_cost) {
				edge_cost[current_neighbor.row][current_neighbor.col] = current_cost;
				best_dir[current_pixel.row][current_pixel.col] = (GRAPH_EDGE_DIR)current_dir;
			}
		}
	}
}

Index MST::determine_best_neighbor(std::vector<std::array<Index, 4>> &neighbor) {
	double best_cost = std::numeric_limits<double>::infinity();
	Index best_neighbor = { -1, -1 };

	for (int current_pixel = 0; current_pixel < neighbor.size(); current_pixel++) {
		for (int dir = 0; dir < 4; dir++) {
	
			Index current_neighbor = neighbor[current_pixel][dir];
			if (current_neighbor.is_none() || visited[current_neighbor.row][current_neighbor.col]) {
				continue;
			}

			double current_cost = edge_cost[current_neighbor.row][current_neighbor.col];

			if (current_cost < best_cost) {
				best_cost = current_cost;
				best_neighbor = current_neighbor;
			}
		}
	}

	return best_neighbor;
}

void MST::build_MST(int row_start, int col_start) {
	mst_set.push_back({ row_start, col_start });
	unsigned int iter = 0;

	while (true) {
		//neighbor.reserve(mst_set.size());
		std::vector<std::array<Index, 4>> neighbor = find_outline();

		update_costs(neighbor);

		Index best_neighbor = determine_best_neighbor(neighbor);
		if (best_neighbor.is_none()) {
			break;
		}

		mst_set.push_back(best_neighbor);
		std::cout << iter++ << std::endl;
	}
}

void MST::genotype_generator(Genotype &genotype) {
	int row_start = rand() % IMAGE_HEIGHT;
	int col_start = rand() % IMAGE_WIDTH;

	build_MST(row_start, col_start);

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			genotype.edge[row][col] = best_dir[row][col];
		}
	}
}

MST::MST() {
	const double INF = std::numeric_limits<double>::infinity();
	edge_cost = new double*[IMAGE_HEIGHT];
	best_dir = new GRAPH_EDGE_DIR*[IMAGE_HEIGHT];
	visited = new bool*[IMAGE_HEIGHT];
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		edge_cost[row] = new double[IMAGE_WIDTH];
		best_dir[row] = new GRAPH_EDGE_DIR[IMAGE_WIDTH];
		visited[row] = new bool[IMAGE_WIDTH];
	}


	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			edge_cost[row][col] = INF;
			best_dir[row][col] = NONE;
			visited[row][col] = false;
		}
	}
}

MST::~MST() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] edge_cost[row];
		delete[] best_dir[row];
		delete[] visited[row];
	}
}
/********************************************************************************/



/*********************************** Genotype ***********************************/
Genotype::Genotype() {
	edge = new GRAPH_EDGE_DIR*[IMAGE_HEIGHT];
	
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		edge[row] = new GRAPH_EDGE_DIR[IMAGE_WIDTH];
	}
}

Genotype::~Genotype() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete [] edge[row];
	}
}

void Genotype::random_init() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			edge[row][col] = (GRAPH_EDGE_DIR)(rand() % 5);
		}
	}
}

//Member functions
void Genotype::mutate() {
	unsigned int row = rand() % IMAGE_HEIGHT;
	unsigned int col = rand() % IMAGE_WIDTH;
	GRAPH_EDGE_DIR dir = static_cast<GRAPH_EDGE_DIR>(rand() % 5);

	edge[row][col] = dir;
}

void Genotype::crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	const GRAPH_EDGE_DIR* p2_ptr = p2[0];
	GRAPH_EDGE_DIR* c1_ptr = c1[0];
	GRAPH_EDGE_DIR* c2_ptr = c2[0];
	
	int crossover_point = rand() % N_PIXELS;
	
	for(int i = 0; i < crossover_point; i++){
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
	}

	for (int i = crossover_point; i < N_PIXELS; i++) {
		c1_ptr[i] = p2_ptr[i];
		c2_ptr[i] = (*edge)[i];
	}
	
}

void Genotype::crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	const GRAPH_EDGE_DIR* p2_ptr = p2[0];
	GRAPH_EDGE_DIR* c1_ptr = c1[0];
	GRAPH_EDGE_DIR* c2_ptr = c2[0];

	int crossover_point2 = rand() % N_PIXELS;
	int crossover_point1 = rand() % crossover_point2;

	for (int i = 0; i < crossover_point1; i++) {
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
	}

	for (int i = crossover_point1; i < crossover_point2; i++) {
		c1_ptr[i] = p2_ptr[i];
		c2_ptr[i] = (*edge)[i];
	}

	for (int i = crossover_point2; i < N_PIXELS; i++) {
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
	}
	
}

std::ostream& operator<<(std::ostream& os, Genotype genotype) {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			switch (genotype.edge[row][col]) {
			case(LEFT):
				os << "<\t";
				break;
			case(RIGHT):
				os << ">\t";
				break;
			case(UP):
				os << "^\t";
				break;
			case(DOWN):
				os << "v\t";
				break;
			case(NONE):
				os << "o\t";
				break;
			}
		}
		os << std::endl;
	}
	return os;
}
/********************************************************************************/


/*
void Phenotype::determine_segment_outlines(std::vector<Segment>& segment) {

for (int row = 0; row < IMAGE_HEIGHT; row++) {
for (int col = 0; col < IMAGE_WIDTH; col++) {

Index neighbor[4];
get_neighbors(row, col, neighbor);
Index current_pixel = { row, col };
int current_pixel_id = belongs_to_segment[current_pixel.row][current_pixel.col];

bool outline_found = false;
for (int i = 0; i < 4; i++) {
Index current_neighbor = neighbor[i];
if (current_neighbor.is_none()) {
continue;
}

int current_neighbor_id = belongs_to_segment[current_neighbor.row][current_neighbor.col];

if (current_neighbor_id != current_pixel_id) {
segment[current_neighbor_id - 1].add_pixel_to_outline(current_neighbor.row, current_neighbor.col);
outline_found = true;
}
}
if (outline_found) {
segment[current_pixel_id - 1].add_pixel_to_outline(current_pixel.row, current_pixel.col);
}

}
}

}*/

/*//test
	std::cout << std::endl << std::endl;
	bool in_temp = false;
	char charr[IMAGE_WIDTH][IMAGE_HEIGHT];
	for (int row2 = 0; row2 < IMAGE_HEIGHT; row2++) {
		for (int col2 = 0; col2 < IMAGE_WIDTH; col2++) {
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i].row == row2 && temp[i].col == col2) {
					in_temp = true;
					switch ((*genotype)[row2][col2]) {
					case(LEFT):
						charr[row2][col2] = '<';
						break;
					case(RIGHT):
						charr[row2][col2] = '>';
						break;
					case(UP):
						charr[row2][col2] = '^';
						break;
					case(DOWN):
						charr[row2][col2] = 'v';
						break;
					case(NONE):
						charr[row2][col2] = 'o';
						break;
					}
					break;
				}
			}
			if (!in_temp) {
				charr[row2][col2] = '-';
			}
			in_temp = false;
		}
	}

	for (int row2 = 0; row2 < IMAGE_HEIGHT; row2++) {
		for (int col2 = 0; col2 < IMAGE_WIDTH; col2++) {
			std::cout << charr[row2][col2] << "\t";
		}
		std::cout << std::endl;
		}*/