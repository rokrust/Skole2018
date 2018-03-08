#include "image.hpp"

#include <array>
#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <limits>

double Pixel::color_distance(Pixel p) {
	Pixel p_diff = *this - p;
	return sqrt(pow(p_diff.r, 2) + pow(p_diff.g, 2) + pow(p_diff.b, 2));
}

double Pixel::color_distance(Pixel p) const {
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

Pixel Pixel::operator-(Pixel p) const {
	Pixel rhs;

	rhs.r = this->r > p.r ? this->r - p.r : p.r - this->r;
	rhs.g = this->g > p.g ? this->g - p.g : p.g - this->g;
	rhs.b = this->b > p.b ? this->b - p.b : p.b - this->b;

	return rhs;
}

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

Image::Image() {
	pixels = new Pixel*[IMAGE_HEIGHT];
	horizontal_color_distance = new double[IMAGE_HEIGHT*(IMAGE_WIDTH-1)];
	vertical_color_distance = new double[(IMAGE_HEIGHT-1)*IMAGE_WIDTH];
	
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		pixels[row] = new Pixel[IMAGE_WIDTH];
	}
}

Image::Image(char* image_dir): Image() {
	this->read(image_dir);
}

Image::~Image() {
	delete vertical_color_distance;
	delete horizontal_color_distance;

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] pixels[row];
	}
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

void Image::calculate_color_distances() const {

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH - 1; col++) {
			horizontal_color_distance[row*(IMAGE_WIDTH - 1) + col] = image[row][col].color_distance(image[row][col + 1]);
		}
	}

	for (int row = 0; row < IMAGE_HEIGHT - 1; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			vertical_color_distance[row*IMAGE_WIDTH + col] = image[row][col].color_distance(image[row + 1][col]);
		}
	}
}

double Image::get_dist(const Index& p1, const Index& p2) const {
	Index p_diff = { p1.row - p2.row, p1.col - p2.col };

	if (p_diff.col == 1) {
		return horizontal_color_distance[p2.row * (IMAGE_WIDTH - 1) + p2.col];
	}
	
	else if (p_diff.col == -1) {
		return horizontal_color_distance[p1.row * (IMAGE_WIDTH - 1) + p1.col];
	}
	
	else if (p_diff.row == -1) {
		return vertical_color_distance[p1.row * (IMAGE_WIDTH - 1) + p1.col];
	}

	else if(p_diff.row == 1) {
		return vertical_color_distance[p2.row * (IMAGE_WIDTH - 1) + p2.col];
	}

	//Pixels are not neighbors
	return std::numeric_limits<double>::infinity();
}

void Image::get_neighbors(int row, int col, std::array<Index, 4> neighbor) const {
	col != 0 ? neighbor[0] = { row, col - 1 } : neighbor[0] = { -1, -1 };
	col != IMAGE_WIDTH - 1 ? neighbor[1] = { row, col + 1 } : neighbor[1] = { -1, -1 };
	row != 0 ? neighbor[2] = { row - 1, col } : neighbor[2] = { -1, -1 };
	row != IMAGE_HEIGHT - 1 ? neighbor[3] = { row + 1, col } : neighbor[3] = { -1, -1 };
}

void Image::read(char* image_dir){
	cv::Mat mat;
	mat = cv::imread(image_dir, CV_LOAD_IMAGE_COLOR);
	if (IMAGE_HEIGHT != mat.rows || IMAGE_WIDTH != mat.cols) {
		std::cout << "IMAGE_HEIGHT and/or IMAGE_WIDTH set to wrong value\n";
	}

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			cv::Vec3b rgb = mat.at<cv::Vec3b>(row, col);
			pixels[row][col] = { rgb[0], rgb[1], rgb[2] };;

		}
	}
}