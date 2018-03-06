#include "image.hpp"

#include <array>
#include <math.h>
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
#include <iostream>

double Pixel::color_distance(Pixel p) {
	Pixel p_diff = *this - p;
	return sqrt(pow(p_diff.r, 2) + pow(p_diff.g, 2) + pow(p_diff.b, 2));
}

Pixel Pixel::operator-(Pixel p) {
	Pixel rhs;

	rhs.r = this->r > p.r ? this->r - p.r : p.r - this->r;
	rhs.g = this->g > p.g ? this->g - p.g : p.g - this->g;
	rhs.b = this->b > p.b ? this->b - p.b : p.b - this->b;

	return rhs;
}

Segment::Segment() { }

Pixel Segment::calculate_centroid() {
	unsigned int sum_r = 0, sum_g = 0, sum_b = 0;
	for (int i = 0; i < this->pixel.size(); i++) {
		Pixel pixel_value = image[pixel[i].row][pixel[i].col];
		sum_r += pixel_value.r;
		sum_g += pixel_value.g;
		sum_b += pixel_value.b;
	}

	Pixel centroid = { 
		(unsigned char)(sum_r / this->pixel.size()), //r
		(unsigned char)(sum_g / this->pixel.size()), //g
		(unsigned char)(sum_b / this->pixel.size())  //b
	};
	
	return centroid;
}

Index Image::next_index(int row, int col, GRAPH_EDGE_DIR dir) const {
	switch (dir) {
	case(LEFT):
		if (col != 0) { return{ row, col - 1 }; }
		break;

	case(RIGHT):
		if (col != IMAGE_WIDTH - 1) { return{ row, col + 1 }; }
		break;

	case(UP):
		if (row != 0) { return{ row - 1, col }; }
		break;

	case(DOWN):
		if (row != IMAGE_HEIGHT - 1) { return{ row + 1, col }; }
		break;
	}

	return{ -1, -1 };
}

/*void Image::get_neighbors(int row, int col, Index* neighbor) const {
	//neighbor[0] = next_index(row, col, LEFT);
	//neighbor[1] = next_index(row, col, RIGHT);
	//neighbor[2] = next_index(row, col, UP);
	//neighbor[3] = next_index(row, col, DOWN);
	col != 0 ? neighbor[0] = { row, col - 1 } : neighbor[0] = { -1, -1 };
	col != IMAGE_WIDTH - 1 ? neighbor[1] = { row, col + 1 } : neighbor[1] = { -1, -1 };
	row != 0 ? neighbor[2] = { row - 1, col } : neighbor[2] = { -1, -1 };
	row != IMAGE_HEIGHT - 1 ? neighbor[3] = { row + 1, col } : neighbor[3] = { -1, -1 };

}*/

void Image::get_neighbors(int row, int col, std::array<Index, 4> neighbor) const {
	col != 0 ? neighbor[0] = { row, col - 1 } : neighbor[0] = { -1, -1 };
	col != IMAGE_WIDTH - 1 ? neighbor[1] = { row, col + 1 } : neighbor[1] = { -1, -1 };
	row != 0 ? neighbor[2] = { row - 1, col } : neighbor[2] = { -1, -1 };
	row != IMAGE_HEIGHT - 1 ? neighbor[3] = { row + 1, col } : neighbor[3] = { -1, -1 };
}