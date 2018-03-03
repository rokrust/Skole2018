#include "Genetic_algorithm.hpp"

Phenotype::Phenotype() {
	//this->_prims_algorithm()
}

Phenotype::Phenotype(const unsigned int & height, const unsigned int & width) {
	//this->genotype = cv::Mat(height, width, CV_8U);
}

Phenotype::Phenotype(Pixel* image, Genotype genotype, const unsigned int & n_pixels) {
	bool visited = new bool[n_pixels];
	std::vector<Segment> current_segment;
	
	//current_segment.push_back()
	//for (int i = 0; i < n_pixels; i++) {
	//	current_segment
	//}
}

void Genotype::create_MST(cv::Mat Image) {
	
}

double Phenotype::calculate_overall_deviation() {
	for (int i = 0; i < n_segments; i++) {
		Segment current_segment = segment[i];
		unsigned int current_segment_length = segment_length[i];

		unsigned int current_deviation = 0;
		Pixel current_centroid = current_segment.calculate_centroid();
		for (int pixel = 0; pixel < current_segment_length; pixel++) {
			//current_deviation += color_distance(current_segment[pixel], current_centroid);
			//color_distance(segment[i], );
		}
	
	}
}

double Phenotype::calculate_edge_value() {

}