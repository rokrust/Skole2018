#include "image.hpp"

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