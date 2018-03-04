#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>

//Helper functions
unsigned int next_index(unsigned int current_index, GRAPH_EDGE_DIR dir) {
	switch (dir) {
	case(LEFT):
		return current_index % IMAGE_WIDTH != 0 ? current_index + 1 : current_index;
	case(RIGHT):
		return current_index % (IMAGE_WIDTH - 1) != 0 ? current_index + 1 : current_index;
	case(UP):
		return current_index > IMAGE_WIDTH ? current_index - IMAGE_WIDTH : current_index;
	case(DOWN):
		return current_index + IMAGE_WIDTH < IMAGE_WIDTH ? current_index + IMAGE_WIDTH : current_index;
	}

}

Index next_index(Index pos, GRAPH_EDGE_DIR dir) {

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

Index* get_neighbors(unsigned int row, unsigned int col) {
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

Phenotype::Phenotype(const unsigned int & height, const unsigned int & width) {
	//this->genotype = cv::Mat(height, width, CV_8U);
}

Phenotype::Phenotype(Genotype genotype) {
	bool is_part_of_segment[IMAGE_HEIGHT][IMAGE_WIDTH];
	bool visited[IMAGE_HEIGHT][IMAGE_WIDTH];
	std::vector<Index>edge;
	bool pixel_added = true;

	//Init
	edge.push_back({ 0, 0 });
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			is_part_of_segment[row][col] = false;
			visited[row][col] = false;
		}
	}

	while(pixel_added){
		pixel_added = false;
		
		//For each pixel at edge
		for (int i = edge.size() - 1; i >= 0 ; i++) {
			Index current_pixel = edge[i];
			GRAPH_EDGE_DIR dir = genotype[current_pixel.row][current_pixel.col];

			if (dir != NONE) {
				Index next_pixel = next_index(current_pixel, dir);
				if (!is_part_of_segment[next_pixel.row][next_pixel.col] && next_pixel.row != -1) {
					is_part_of_segment[next_pixel.row][next_pixel.col] = true;
					edge.push_back(next_pixel);
				}
			}
		}

	}
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
	unsigned int row = rand() % IMAGE_HEIGHT;
	unsigned int col = rand() % IMAGE_WIDTH;
	GRAPH_EDGE_DIR dir = static_cast<GRAPH_EDGE_DIR>(rand() % 5);

	edge[row][col] = dir;
}

void Genotype::crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	unsigned int row_point = rand() % IMAGE_HEIGHT;
	unsigned int col_point = rand() % IMAGE_WIDTH;
	
	for (unsigned int row = 0; row < row_point-1; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = (*this)[row][col];
			c2[row][col] = p2[row][col];
		}
	}

	for (unsigned int row = row_point; row < IMAGE_HEIGHT; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = p2[row][col];
			c2[row][col] = (*this)[row][col];
		}
	}
}

void Genotype::crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	unsigned int row_point2 = rand() % IMAGE_HEIGHT;
	unsigned int row_point1 = rand() % row_point1;
	unsigned int col_point1 = rand() % IMAGE_WIDTH;
	unsigned int col_point2 = rand() % IMAGE_WIDTH;

	for (unsigned int row = 0; row < row_point1 - 1; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = (*this)[row][col];
			c2[row][col] = p2[row][col];
		}
	}

	for (unsigned int row = row_point1 + 1; row < row_point2 - 1; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = p2[row][col];
			c2[row][col] = (*this)[row][col];
		}
	}

	for (unsigned int row = row_point2 + 1; row < IMAGE_WIDTH; row++) {
		for (unsigned int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = (*this)[row][col];
			c2[row][col] = p2[row][col];
		}
	}

	for (unsigned int col = 0; col < col_point1; col++) {
		c1[row_point1][col] = (*this)[row_point1][col];
		c2[row_point1][col] = p2[row_point1][col];
	}

	for (unsigned int col = col_point1; col < IMAGE_WIDTH; col++) {
		c1[row_point2][col] = p2[row_point1][col];
		c2[row_point2][col] = (*this)[row_point1][col];
	}

	for (unsigned int col = 0; col < col_point2; col++) {
		c1[row_point2][col] = p2[row_point2][col];
		c2[row_point2][col] = (*this)[row_point2][col];
	}
	
	for (unsigned int col = col_point2; col < IMAGE_WIDTH; col++) {
		c1[row_point1][col] = (*this)[row_point1][col];
		c2[row_point1][col] = p2[row_point1][col];
	}

}
