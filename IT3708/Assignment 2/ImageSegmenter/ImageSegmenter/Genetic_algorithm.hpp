#pragma once
#include "image.hpp"
#include <vector>
#include <opencv2/core.hpp>

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

class Genotype {
private:
	GRAPH_EDGE_DIR* edge;

public:
	Genotype() { edge = nullptr; }
	Genotype(unsigned int height, unsigned int width) { edge = new GRAPH_EDGE_DIR[height*width]; }
	Genotype(cv::Mat Image);

	~Genotype() { delete edge; }

	void create_MST(cv::Mat Image);

};

class Phenotype {
private:
	std::vector<Segment*> segment;
	unsigned int n_segments;
	unsigned int* segment_length;
	
public:
	Phenotype();
	Phenotype(Pixels* image, Genotype genotype, const unsigned int & n_pixels);	//Initial greedy segmentation
	Phenotype(const unsigned int & height, const unsigned int & width);
	
	// Minimization objectives
	double calculate_overall_deviation();
	double calculate_edge_value();
};