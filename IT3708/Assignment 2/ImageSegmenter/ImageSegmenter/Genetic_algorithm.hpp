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

	//Genetic operators
	void mutate();
	void crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2);
	void crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2);

	//Overloaded operators
	GRAPH_EDGE_DIR & operator[](unsigned int index) { return edge[index]; }
	const GRAPH_EDGE_DIR & operator[](unsigned int index) const { return edge[index]; }
};

class Phenotype {
private:
	std::vector<Segment> segment;
	
public:
	Phenotype();
	Phenotype(Genotype genotype, const Image & image);
	Phenotype(const unsigned int & height, const unsigned int & width);

	// Minimization objectives
	double calculate_overall_deviation();
	double calculate_edge_value();
};