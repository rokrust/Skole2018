#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>

//Helper functions
unsigned int next_index(unsigned int current_index, GRAPH_EDGE_DIR dir) {

	switch (dir) {
	case(LEFT):
		if (current_index % IMAGE_WIDTH == 0) { // In first column
			return current_index;
		}
		return current_index - 1;

	case(RIGHT):
		if (current_index % IMAGE_WIDTH - 1 == 0) { // In last column
			return current_index;
		}
		return current_index + 1;

	case(UP):
		if (current_index < IMAGE_WIDTH) { // In first row
			return current_index;
		}
		return current_index - IMAGE_WIDTH;
	
	case(DOWN):
		if (current_index < IMAGE_WIDTH*(IMAGE_HEIGHT - 1)) { //In last row
			return current_index;
		}
		return current_index + IMAGE_WIDTH;

	case(NONE):
		return current_index;
	}

}

Index next_index(unsigned int row, unsigned int col, GRAPH_EDGE_DIR dir) {

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

	return { row, col };
}

Index* get_neighbors(unsigned int row, unsigned int col) {
	Index temp[4];

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

Phenotype::Phenotype(const unsigned int & height, const unsigned int & width) {
	//this->genotype = cv::Mat(height, width, CV_8U);
}

Phenotype::Phenotype(Genotype genotype, const Image & image) {
	bool* visited = new bool [N_PIXELS];
	std::vector<Pixel> current_segment;
	std::vector<unsigned int> too_be_visited;

	//Initialize
	current_segment.push_back(image[0]);
	visited[0] = true;
	
	//Iterate through image graph
	for (unsigned int row = 0; row < IMAGE_HEIGHT; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			unsigned int i = row*IMAGE_HEIGHT + col;
			GRAPH_EDGE_DIR dir = genotype[i];
			Index next_i = next_index(row, col, dir);

			Index* neighbor = get_neighbors(row, col);

			for (unsigned int j = 0; i < 4; i++) {
				if (neighbor[j].x != row || neighbor[j].y != col) {
					too_be_visited.push_back(neighbor[j].x*IMAGE_HEIGHT + neighbor[j].y);
				}
			}
		
		}
	}
}

//Member functions
double Phenotype::calculate_overall_deviation() {
	unsigned int overall_deviation = 0;

	for (std::vector<Segment>::iterator current_segment = segment.begin();
		current_segment != segment.end();
		current_segment++) {

		unsigned int current_deviation = 0;
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

}

/* Genotype */
//Member functions
void Genotype::mutate() {
	unsigned int i = rand() % IMAGE_HEIGHT*IMAGE_WIDTH;
	GRAPH_EDGE_DIR dir = static_cast<GRAPH_EDGE_DIR>(rand() % 5);

	edge[i] = dir;
}

void Genotype::crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	unsigned int i = rand() % N_PIXELS;

	for (unsigned int k = 0; k < i; k++) {
		c1[k] = p2[k];
		c2[k] = (*this)[k];
	}

	for (unsigned int k = i; i < N_PIXELS; k++) {
		c1[k] = (*this)[k];
		c2[k] = p2[k];
	}
}

void Genotype::crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	unsigned int i = rand() % N_PIXELS;
	unsigned int j = rand() % i;

	for (unsigned int k = 0; k < j; k++) {
		c1[k] = (*this)[k];
		c2[k] = p2[k];
	}

	for (unsigned int k = j; k < i; k++) {
		c1[k] = p2[k];
		c2[k] = (*this)[k];
	}

	for (unsigned int k = i; i < N_PIXELS; k++) {
		c1[k] = (*this)[k];
		c2[k] = p2[k];
	}
}

