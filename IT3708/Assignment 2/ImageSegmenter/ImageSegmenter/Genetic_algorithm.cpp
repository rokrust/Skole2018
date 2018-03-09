#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>
#include <limits>
#include <array>
#include <chrono>
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

		// Add to segment
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

	using namespace std::chrono;
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	// As long as a pixel was added to the segment
	t1 = high_resolution_clock::now();
	while (active_edge.size() != 0) {
		new_active_edge.clear();
		
		for (int i = active_edge.size() - 1; i >= 0; i--) {
			Index current_pixel = active_edge[i];
			
			current_segment.add_pixel(current_pixel.row, current_pixel.col);
			active_edge.pop_back();
			belongs_to_segment[current_pixel.row][current_pixel.col] = segment_id;
			
			add_ingoing_to_active_edge(current_pixel.row, current_pixel.col);	//Kalles mange ganger på samme pixel?

			//Add all neighbors pointing towards the segment

			//Add all neighbors being pointed to by the segment
			GRAPH_EDGE_DIR dir = (*genotype)[current_pixel.row][current_pixel.col];
			add_outgoing_to_active_edge(current_pixel.row, current_pixel.col, dir);

		}

		//All members of the old edge handled. Go on to new edge
		active_edge = new_active_edge;
	}
	t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(t2 - t1).count();
	std::cout << duration << std::endl;

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
			std::array<Index, 4> neighbor = {};

			image.get_neighbors(row, col, neighbor);
			Index current_pixel = { row, col };
			int current_pixel_id = belongs_to_segment[current_pixel.row][current_pixel.col];

			for (int i = 0; i < 4; i++) {
				Index current_neighbor = neighbor[i];
				if (current_neighbor.is_none()) { continue; }
				
				int current_neighbor_id = belongs_to_segment[current_neighbor.row][current_neighbor.col];

				if (current_neighbor_id != current_pixel_id) {
					edge_value += image.distance_between(current_pixel, current_neighbor);
				}
			}
		}
	}

	return edge_value;
}

/********************************************************************************/


/************************************* MST **************************************/
void MST::build_tree(Index start_node) {
	using namespace std::chrono;
	add_node(start_node);
	Index closest_node = start_node;
	
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	duration<double> time_span;
	int i = 0;
	std::cout << '(' << start_node.row << ", " << start_node.col << ')' << std::endl;

	while (true){//outline.size() > 0) {
		//if (i++ % 1000 == 0) {
		//	std::cout << outline.size() << std::endl;
		//}
		//t1 = high_resolution_clock::now();
		update_costs(closest_node);	
		//t2 = high_resolution_clock::now();
		//time_span = duration_cast<duration<double>>(t2 - t1);
		
		//std::cout << time_span.count() << std::endl;
		
		//t1 = high_resolution_clock::now();
		closest_node = find_closest_node();
		//t2 = high_resolution_clock::now();
		//time_span = duration_cast<duration<double>>(t2 - t1);

		//std::cout << time_span.count() << std::endl;
		if (closest_node.is_none()) { 
			break; 
		}

		//t1 = high_resolution_clock::now();
		add_node(closest_node);
		//t2 = high_resolution_clock::now();
		//time_span = duration_cast<duration<double>>(t2 - t1);

		//std::cout << time_span.count() << std::endl << std::endl;

	}

}

void MST::update_costs(Index current_node) {
	std::array<Index, 4> neighbor = {};
	image.get_neighbors(current_node.row, current_node.col, neighbor);
	
	for (int j = 0; j < 4; j++) {
		if (neighbor[j].is_none()) { continue; }
		if (in_tree[neighbor[j].row][neighbor[j].col]) { continue; }

		double dist = image.distance_between(current_node, neighbor[j]);
		if (dist < edge_cost[neighbor[j].row][neighbor[j].col]) {
			edge_cost[neighbor[j].row][neighbor[j].col] = dist;
			dir[neighbor[j].row][neighbor[j].col] = neighbor[j].get_direction_to(current_node);
		}
	}
}

Index MST::find_closest_node() {
	Index closest_node = { -1, -1 };

	//Iterate through the outline
	for (int i = 0; i < outline.size(); i++) {
		Index current_node = outline[i];

		//Find all the neighbors of the outline
		std::array<Index, 4> neighbor = {};
		image.get_neighbors(current_node.row, current_node.col, neighbor);

		//Iterate through the outlining neighbors and pick the closest one
		double lowest_cost = std::numeric_limits<double>::infinity();
		for (int j = 0; j < 4; j++) {
			if (neighbor[j].is_none()) { continue; }
			if (in_tree[neighbor[j].row][neighbor[j].col]) { continue; }

			if (edge_cost[neighbor[j].row][neighbor[j].col] < lowest_cost) {
				lowest_cost = edge_cost[neighbor[j].row][neighbor[j].col];
				closest_node = neighbor[j];
			}
		}
	}

	return closest_node;
}

void MST::add_node(Index node) {
	//Add the closest node to the tree
	mst_set.push_back(node);
	in_tree[node.row][node.col] = true;

	if (unused_neighbors[node.row][node.col] != 0) {
		outline.push_back(node);
	}

	pop_surrounding_caged_nodes(node);
}

void MST::pop_surrounding_caged_nodes(Index node) {
	std::array<Index, 4> neighbor = {};
	image.get_neighbors(node.row, node.col, neighbor);

	//See if any of the neighbors of the new node are caged
	for (int j = 0; j < 4; j++) {
		if (neighbor[j].is_none()) { continue; } //Neighbor is out of bounds
		unused_neighbors[neighbor[j].row][neighbor[j].col]--;

		if (!in_tree[neighbor[j].row][neighbor[j].col]) { continue; } //Node is not in the tree yet

		//Neighbor[j] is caged in
		if (unused_neighbors[neighbor[j].row][neighbor[j].col] == 0) {
			
			//Find neighbor[j] in the outline and erase
			for (int i = 0; i < outline.size(); i++) {
				if (outline[i] == neighbor[j]) {
					outline.erase(outline.begin() + i);
					break;
				}

			}

		}
	}
}

void MST::genotype_generator(Genotype& genotype) {
	Index start_node = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
	build_tree(start_node);

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			genotype.edge[row][col] = dir[row][col];
		}
	}
}

MST::MST() {
	const double INF = std::numeric_limits<double>::infinity();
	edge_cost = new double*[IMAGE_HEIGHT];
	dir = new GRAPH_EDGE_DIR*[IMAGE_HEIGHT];
	in_tree = new bool*[IMAGE_HEIGHT];
	unused_neighbors = new unsigned char*[IMAGE_HEIGHT];

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		edge_cost[row] = new double[IMAGE_WIDTH];
		dir[row] = new GRAPH_EDGE_DIR[IMAGE_WIDTH];
		in_tree[row] = new bool[IMAGE_WIDTH];
		unused_neighbors[row] = new unsigned char[IMAGE_WIDTH];
	}


	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			edge_cost[row][col] = INF;
			dir[row][col] = NONE;
			in_tree[row][col] = false;
			unused_neighbors[row][col] = 4;

		}
	}
	for (int row = 1; row < IMAGE_HEIGHT; row++) {
		unused_neighbors[row][0] = 3;
		unused_neighbors[row][IMAGE_WIDTH - 1] = 3;
	}
	
	for (int col = 1; col < IMAGE_WIDTH; col++) {
		unused_neighbors[0][col] = 3;
		unused_neighbors[IMAGE_HEIGHT - 1][col] = 3;
	}

	unused_neighbors[0][0] = 2;
	unused_neighbors[0][IMAGE_WIDTH - 1] = 2;
	unused_neighbors[IMAGE_HEIGHT - 1][0] = 2;
	unused_neighbors[IMAGE_HEIGHT - 1][IMAGE_WIDTH - 1] = 2;
}

MST::~MST() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] edge_cost[row];
		delete[] dir[row];
		delete[] in_tree[row];
		delete[] unused_neighbors[row];
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