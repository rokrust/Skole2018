#pragma once
#include "image.hpp"
#include <vector>
//#include <opencv2/core.hpp>
#include <iostream>

class Genotype; class Phenotype;

class Genotype {
private:
	GRAPH_EDGE_DIR** edge;

public:
	Genotype();
	Genotype(const Genotype& original);
	~Genotype();

	//Genetic operators
	void mutate(); //okay
	void crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2); //okay
	void crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2); //okay

	//Overloaded operators
	GRAPH_EDGE_DIR* operator[](unsigned int row) { return edge[row]; } //okay
	const GRAPH_EDGE_DIR* operator[](unsigned int row) const { return edge[row]; } //okay

	Genotype& operator=(const Genotype &rhs);

	friend std::ostream& operator<<(std::ostream& os, Genotype genotype);
	friend class MST;
};

struct Node { double cost; GRAPH_EDGE_DIR dir; Index index; };

class MST {

private:
	bool** in_tree;
	
	std::vector<Node> mst_set;
	std::vector<Node> priority_queue;
public:
	MST();
	MST(const MST &mst);
	~MST();

	void insert_in_queue(Node node);
	Node pop_from_queue();
	void add_neighbors_to_queue(Node node);
	void add_node_to_mst(Node closest_node);
	void genotype_generator(Genotype& genotype, Index root_node);
	void build_tree(Index start_node);

	MST& operator=(const MST &rhs);
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

class Population {
private:
	int archive_size;
	double mutation_rate, crossover_rate;
	std::vector<Genotype> population;

public:
	Population() {}
	Population(int population_size, int archive_size, double mutation_rate, double crossover_rate, int n_concurrent_threads = 4);

	void next_generation();
	void tournament_selection();
	void find_pareto_fronts();
	Genotype get_genotype(int i) { return population[i]; }
};
