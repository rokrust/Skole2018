#include "Gentic_algorithm.hpp"

Phenotype::Phenotype() {
	this->_prims_algorithm()
}

Phenotype::Phenotype(unsigned int height, unsigned int width) {
	this->genotype = cv::Mat(height, width, CV_8U);
}

void Phenotype::_prims_algorithm() {
	
}

double Phenotype::calculate_overall_deviation(Image* segment, unsigned int* segment_length, unsigned int n_segments) {
	unsigned int 

	for (int i = 0; i < n_segments; i++) {
		Image current_segment = segment[i];
		unsigned int current_segment_length = segment_length[i];

		unsigned int current_deviation = 0;
		Pixel current_centroid = current_segment.calculate_centroid();
		for (int pixel = 0; pixel < current_segment_length; pixel++) {
			//current_deviation += color_distance(current_segment[pixel], current_centroid);
			//color_distance(segment[i], );
		}
	
	}
}

double Phenotype::calculate_edge_value(Pixel* segment) {

}