#pragma once
#include "image.hpp"
#include <vector>
//#include <opencv2/core.hpp>
#include <iostream>

class Genotype; class Phenotype;

class Population {
private:
	int archive_size;
	std::vector<Genotype> population;

public:
	Population() {}
	Population(int population_size, int archive_size);

	void next_generation();
	void tournament_selection();
	void find_pareto_fronts();
};

class Genotype {
private:
	GRAPH_EDGE_DIR** edge;

public:
	Genotype();
	~Genotype();

	void random_init();

	//Genetic operators
	void mutate(); //okay
	void crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2); //okay
	void crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2); //okay

	//Overloaded operators
	GRAPH_EDGE_DIR* operator[](unsigned int row) { return edge[row]; } //okay
	const GRAPH_EDGE_DIR* operator[](unsigned int row) const { return edge[row]; } //okay

	void test() {
		for (int row = 0; row < IMAGE_HEIGHT; row++) {
			for (int col = 0; col < IMAGE_WIDTH; col++) {
				edge[row][col] = (GRAPH_EDGE_DIR)(rand() % 5);
			}
		}
	}
	friend std::ostream& operator<<(std::ostream& os, Genotype genotype);
	friend class MST;
};

class MST {
private:
	double** edge_cost;
	GRAPH_EDGE_DIR** best_dir;
	bool** visited;
	std::vector<Index> mst_set;

public:
	MST();
	~MST();

	void genotype_generator(Genotype& genotype);

	void update_costs(std::vector<std::array<Index, 4>>& neighbor);
	Index determine_best_neighbor(std::vector <std::array<Index, 4>>& neighbor);
	std::vector<std::array<Index, 4>> find_outline();
	void build_MST(int row_start, int col_start);
};

class Phenotype {
private:
	bool** is_part_of_segment;
	int** belongs_to_segment;
	std::vector<Index> active_edge; //growing edge
	std::vector<Index> new_active_edge;

	const Genotype* genotype;

	std::vector<Segment> segment;

public:
	//Constructors
	Phenotype();
	Phenotype(const Genotype& genotype);
	~Phenotype();

	//Constructor helper functions
	void build_segments();
	void build_segment_from_pixel(int row, int col);
	void add_ingoing_to_active_edge(int row, int col);
	void add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir);
	
	double calculate_overall_deviation();
	double calculate_edge_value();
};
