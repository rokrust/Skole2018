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

	void add_random_root_nodes();

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
	double edge_value, overall_deviation;

public:
	//Constructors
	Phenotype();
	Phenotype(const Genotype& genotype);
	Phenotype(const Phenotype& genotype);
	~Phenotype();

	//Constructor helper functions
	void build_segments();
	void build_segment_from_pixel(int row, int col, int segment_id);
	void add_ingoing_to_active_edge(int row, int col);
	void add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir);
	
	void print_segments();
	bool dominates(const Phenotype& p2);

	void calculate_overall_deviation();
	void calculate_edge_value();
	double get_overall_deviation() { return overall_deviation; }
	double get_edge_value() { return edge_value; }

	Phenotype& operator=(const Phenotype& rhs);
};

class Population {
private:
	int archive_size, tournament_size;
	double mutation_rate, crossover_rate;
	
	std::vector<Genotype> population;
	std::vector<Phenotype> population_phenotypes;
	std::vector<std::vector<Phenotype*>> pareto_fronts; //The individuals are grouped in a set of pareto fronts

public:
	Population() {}
	Population(int population_size, int archive_size, int tournament_size, 
			   double mutation_rate, double crossover_rate, 
			   int n_concurrent_threads = 4);

	void next_generation();
	Phenotype* tournament_selection();
	void non_dominated_sort();
	void crowding_distance();
	void create_phenotypes();
	Genotype get_genotype(int i) { return population[i]; }
};
