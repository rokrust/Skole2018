#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>

/***************************** PhenotypeGenerator *****************************/
/*unsigned int PhenotypeGenerator::next_index(unsigned int current_index, GRAPH_EDGE_DIR dir) {
	switch (dir) {
	case(LEFT):
		return current_index % IMAGE_WIDTH != 0 ? current_index + 1 : current_index;
	case(RIGHT):
		return current_index % (IMAGE_WIDTH - 1) != 0 ? current_index + 1 : current_index;
	case(UP):
		return current_index > IMAGE_WIDTH ? current_index - IMAGE_WIDTH : current_index;
	case(DOWN):
		return current_index + IMAGE_WIDTH < IMAGE_WIDTH ? current_index + IMAGE_WIDTH : current_index;
	case(NONE):
		return -1;
	}

}*/


Index PhenotypeGenerator::next_index(int row, int col, GRAPH_EDGE_DIR dir) {
	switch (dir) {
	case(LEFT):
		if (col != 0) { return { row, col - 1 }; }
		break;

	case(RIGHT):
		if (col != IMAGE_WIDTH - 1) { return { row, col + 1 }; }
		break;

	case(UP):
		if (row != 0) { return { row - 1, col }; }
		break;

	case(DOWN):
		if (row != IMAGE_HEIGHT - 1) { return { row + 1, col }; }
		break;
	}

	return { -1, -1 };
}

void PhenotypeGenerator::get_neighbors(int row, int col, Index* neighbor) {
	//neighbor[0] = next_index(row, col, LEFT);
	//neighbor[1] = next_index(row, col, RIGHT);
	//neighbor[2] = next_index(row, col, UP);
	//neighbor[3] = next_index(row, col, DOWN);
	col != 0 ? neighbor[0] = { row, col - 1 } : neighbor[0] = { -1, -1 };
	col != IMAGE_WIDTH - 1 ? neighbor[1] = { row, col + 1 } : neighbor[1] = { -1, -1 };
	row != 0 ? neighbor[2] = { row - 1, col } : neighbor[2] = { -1, -1 };
	row != IMAGE_HEIGHT - 1 ? neighbor[3] = { row + 1, col } : neighbor[3] = { -1, -1 };

}

void PhenotypeGenerator::add_ingoing_to_active_edge(int row, int col) {
	Index neighbor[4]; 
	get_neighbors(row, col, neighbor);

	for (int pixel = 0; pixel < 4; pixel++) {
		Index current_neighbor = neighbor[pixel];
		if (current_neighbor.is_none()) {
			continue;
		}
		
		GRAPH_EDGE_DIR dir = (*genotype)[current_neighbor.row][current_neighbor.col]; //garbage
		Index next_neighbor = next_index(current_neighbor.row, current_neighbor.col, dir);
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

void PhenotypeGenerator::add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir) {
	if (dir != NONE) {
		Index next_pixel = next_index(row, col, dir);
		if (!(is_part_of_segment[next_pixel.row][next_pixel.col] || next_pixel.is_none())) {
			is_part_of_segment[next_pixel.row][next_pixel.col] = true;
			new_active_edge.push_back(next_pixel);
		}

	}
}
/*
std::vector<Index> PhenotypeGenerator::advance_active_edge() {
	std::vector<Index> new_active_edge;
	for (int i = active_edge.size() - 1; i >= 0; i--) {
		Index current_pixel = active_edge[i];

		temp.push_back(current_pixel); //test
		active_edge.pop_back();

		//Add all neighbors pointing towards the segment
		add_ingoing_to_active_edge(current_pixel.row, current_pixel.col);	
		
		//Add all neighbors being pointed to by the segment
		GRAPH_EDGE_DIR dir = (*genotype)[current_pixel.row][current_pixel.col];
		add_outgoing_to_active_edge(current_pixel.row, current_pixel.col, dir);

	}

	return new_active_edge;
}*/

Segment PhenotypeGenerator::build_segment_from_pixel(int row, int col) {
	active_edge.push_back({ row, col });
	is_part_of_segment[row][col] = true;
	Segment segment;
	
	static int segment_id = 0;
	segment_id++;

	// As long as a pixel was added to the segment
	while (active_edge.size() != 0) {
		new_active_edge.clear();
		for (int i = active_edge.size() - 1; i >= 0; i--) {
			Index current_pixel = active_edge[i];
			
			segment.add_pixel(current_pixel.row, current_pixel.col);
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

	

	return segment;
}

std::vector<Segment> PhenotypeGenerator::build_segments() {
	std::vector<Segment> segments;
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			//Found a pixel that is not a part of a segment
			if (!is_part_of_segment[row][col]) {
				Segment current_segment = build_segment_from_pixel(row, col);
				segments.push_back(current_segment);
			}

		}
	}

	return segments;
}

PhenotypeGenerator::PhenotypeGenerator() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			is_part_of_segment[row][col] = false;
			belongs_to_segment[row][col] = -1;
		}
	}
}

PhenotypeGenerator::PhenotypeGenerator(const Genotype* genotype): PhenotypeGenerator() {
	this->genotype = genotype;
}

/*
void PhenotypeGenerator::determine_segment_outlines(std::vector<Segment>& segment) {

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
/*****************************************************************************/


/******************************** Phenotype **********************************/
//Constructors
Phenotype::Phenotype() {
	//this->_prims_algorithm()
}

Phenotype::Phenotype(const Genotype* genotype) {
	PhenotypeGenerator generator(genotype);
	segment = generator.build_segments();
}

//Member functions
/*double Phenotype::calculate_overall_deviation() {
	double overall_deviation = 0;

	for (std::vector<Segment>::iterator current_segment = segment.begin();
		current_segment != segment.end();
		current_segment++) {

		double current_deviation = 0;
		Pixel current_centroid = current_segment->calculate_centroid();
		
		for (std::vector<Pixel>::iterator pixel = current_segment->begin();
			pixel != current_segment->end();
			pixel++) {

			current_deviation += pixel->color_distance(current_centroid);
		}

		overall_deviation += current_deviation;
	
	}

	return overall_deviation;
}*/

double PhenotypeGenerator::calculate_edge_value() {
	double edge_value = 0.0;

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {

			Index neighbor[4];
			get_neighbors(row, col, neighbor);
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
/******************************************************************************/



/********************************** Genotype **********************************/

//OKAY!
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
/******************************************************************************/

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