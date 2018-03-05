#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>

//Helper functions
unsigned int PhenotypeGenerator::next_index(unsigned int current_index, GRAPH_EDGE_DIR dir) {
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

}

Index PhenotypeGenerator::next_index(Index pos, GRAPH_EDGE_DIR dir) {

	switch (dir) {
	case(LEFT):
		if (pos.col != 0) { return { pos.row, pos.col - 1 }; }
		break;

	case(RIGHT):
		if (pos.col != IMAGE_WIDTH - 1) { return { pos.row, pos.col + 1 }; }
		break;

	case(UP):
		if (pos.row != 0) { return { pos.row - 1, pos.col }; }
		break;

	case(DOWN):
		if (pos.row != IMAGE_HEIGHT - 1) { return { pos.row + 1, pos.col }; }
		break;
	}

	return { -1, -1 };
}

Index* PhenotypeGenerator::get_neighbors(int row, int col) {
	Index temp[4];

	//temp[0] = next_index(row, col, LEFT);
	//temp[1] = next_index(row, col, RIGHT);
	//temp[2] = next_index(row, col, UP);
	//temp[3] = next_index(row, col, DOWN);
	col != 0 ? temp[0] = { row, col - 1 } : temp[0] = { -1, -1 };
	col != IMAGE_WIDTH - 1 ? temp[1] = { row, col + 1 } : temp[1] = { -1, -1 };
	row != 0 ? temp[2] = { row - 1, col } : temp[2] = { -1, -1 };
	row != IMAGE_HEIGHT - 1 ? temp[3] = { row + 1, col } : temp[3] = { -1, -1 };

	return temp;
}

/* Phenotype */
//Constructors
Phenotype::Phenotype() {
	//this->_prims_algorithm()
}

void PhenotypeGenerator::advance_active_edge(Genotype genotype) {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			if (!is_part_of_segment[row][col]) {
				active_edge.push_back({ row, col });

				while (pixel_added) {
					pixel_added = false;

					//For each pixel at inner edge
					for (int i = active_edge.size() - 1; i >= 0; i++) {
						Index current_pixel = active_edge[i];
						Index* neighbor = get_neighbors(current_pixel.row, current_pixel.col);

						for (int pixel = 0; pixel < 4; pixel++) {
							Index current_neighbor = neighbor[pixel];
							if (current_neighbor.is_none()) {
								continue;
							}

							Index next_neighbor = next_index(current_neighbor, genotype[current_neighbor.row][current_neighbor.col]);
							if (next_neighbor.is_none()) {
								continue;
							}

							//Add neighbor if it points to the segment
							if (!is_part_of_segment[current_neighbor.row][current_neighbor.col] &&
								is_part_of_segment[next_neighbor.row][next_neighbor.col]) {
								pixel_added = true;
								is_part_of_segment[current_neighbor.row][current_neighbor.col] = true;
								active_edge.push_back(current_neighbor);
							}
						}


						GRAPH_EDGE_DIR dir = genotype[current_pixel.row][current_pixel.col];
						if (dir != NONE) {
							Index next_pixel = next_index(current_pixel, dir);
							if (!(is_part_of_segment[next_pixel.row][next_pixel.col] || next_pixel.is_none())) {
								pixel_added = true;
								is_part_of_segment[next_pixel.row][next_pixel.col] = true;
								active_edge.push_back(next_pixel);
							}

						}

					}

				}

			}

		}

	}

}

Phenotype::Phenotype(const unsigned int & height, const unsigned int & width) {
	//this->genotype = cv::Mat(height, width, CV_8U);
}

PhenotypeGenerator::PhenotypeGenerator() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			is_part_of_segment[row][col] = false;
			visited[row][col] = false;
		}
	}
}

Phenotype::Phenotype(Genotype genotype) {
	PhenotypeGenerator generator;
	generator.advance_active_edge(genotype);
	//BFS:
	//Begin at some pixel
	//Until no pixels are added:
		//Construct an edge set of all pixels at the edge of the segment
		//For all pixels at edge:
			//If dir not none and next_pixel not visited
				//Pixel added = true
				//Add next_pixel to visited
				//Add pixel to segment
		//For all neighbors of edge:
			//If pixel not visited and point in
				//Pixel added = true
				//Add pixel to visited 
				//Add pixel to segment
		

	//DFS:
	//Until all pixels visited
		//Follow dir until end
			//Add each pixel on the way to segmen
			//if pixel already visited
				//Merge segments
		//Go to next unvisited pixel and start new segment
}

//Member functions
double Phenotype::calculate_overall_deviation() {
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
}

double Phenotype::calculate_edge_value() {
	return 0.0;
}

/* Genotype */
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
	
	unsigned int crossover_point = rand() % N_PIXELS;
	
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

	unsigned int crossover_point2 = rand() % N_PIXELS;
	unsigned int crossover_point1 = rand() % crossover_point2;

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
				os << "_\t";
				break;
			case(NONE):
				os << ".\t";
				break;
			}
		}
		os << std::endl;
	}
	return os;
}
