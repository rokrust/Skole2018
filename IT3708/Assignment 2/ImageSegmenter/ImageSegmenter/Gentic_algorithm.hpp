#pragma once
#include "Gentic_algorithm.hpp"
#include "image.hpp"
#include <opencv2/core.hpp>

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

class Phenotype {
private:
	cv::Mat genotype;

public:
	Phenotype();
	Phenotype(unsigned int height, unsigned int width);

	void _prims_algorithm();
	
	// Minimization objectives
	double calculate_overall_deviation(Image* segment, unsigned int* segment_length, unsigned int n_segments);
	double calculate_edge_value(Image* segment, unsigned int* segment_length, unsigned int n_segments);
};