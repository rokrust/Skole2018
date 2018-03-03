#include "image.hpp"

#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

Image::Image() { 
	picture = nullptr; 
}

Image::Image(char* image_dir) { 

	picture = nullptr;
}

Pixel Image::calculate_centroid(Pixel* segment, unsigned int segment_length) {
	unsigned int sum_r = 0, sum_g = 0, sum_b = 0;
	for (int i = 0; i < segment_length; i++) {
		sum_r += segment[i].get_r();
		sum_g += segment[i].get_g();
		sum_b += segment[i].get_b();
	}

	Pixel centroid = { 
		(unsigned char)(sum_r / segment_length), //r
		(unsigned char)(sum_g / segment_length), //g
		(unsigned char)(sum_b / segment_length)  //b
	};

	return centroid;
}