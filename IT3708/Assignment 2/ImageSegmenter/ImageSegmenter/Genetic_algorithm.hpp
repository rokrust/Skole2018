#pragma once
#include "image.hpp"
#include <vector>
#include <opencv2/core.hpp>

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

class Genotype {
private:
	GRAPH_EDGE_DIR edge[IMAGE_HEIGHT][IMAGE_WIDTH];

public:
	Genotype() { }
	Genotype(cv::Mat Image);

	~Genotype() { delete edge; }

	//Genetic operators
	void mutate();
	void crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2);
	void crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2);

	//Overloaded operators
	GRAPH_EDGE_DIR* operator[](unsigned int row) { return edge[row]; }
	const GRAPH_EDGE_DIR* operator[](unsigned int row) const { return edge[row]; }
	//const GRAPH_EDGE_DIR & operator[](unsigned int index) const { return edge[index]; }
	const GRAPH_EDGE_DIR & operator[](Index index) const { return edge[index.row][index.col]; }

};

class Phenotype {
private:
	std::vector<Segment> segment;
	
public:
	Phenotype();
	Phenotype(Genotype genotype);
	Phenotype(const unsigned int & height, const unsigned int & width);

	// Minimization objectives
	double calculate_overall_deviation();
	double calculate_edge_value();
};