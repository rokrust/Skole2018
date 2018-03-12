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

	std::vector<Segment> segment;
	double edge_value, overall_deviation, crowding_distance;

public:
	int index;

	//Constructors
	Phenotype();
	Phenotype(Genotype& genotype);
	Phenotype(const Phenotype& genotype);
	~Phenotype();

	//Constructor helper functions
	void build_segments(const Genotype& genotype);
	void build_segment_from_pixel(int row, int col, int segment_id, const Genotype& genotype);
	void add_ingoing_to_active_edge(int row, int col, const Genotype& genotype);
	void add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir);
	
	void print_segments();
	bool dominates(const Phenotype& p2);

	void calculate_overall_deviation();
	void calculate_edge_value();
	
	double get_overall_deviation() { return overall_deviation; }
	double get_edge_value() { return edge_value; }
	double get_crowding_distance() { return crowding_distance; }
	void set_crowding_distance(double crowding_distance) { this->crowding_distance = crowding_distance; }

	Phenotype& operator=(const Phenotype& rhs);
};

class Population {
private:
	int tournament_size, population_size;
	double mutation_rate, crossover_rate;
	
	std::vector<Genotype> population;
	std::vector<Phenotype> population_phenotypes;
	std::vector<std::vector<int>> pareto_fronts; //The individuals are grouped in a set of pareto fronts


public:
	Population() {}
	Population(int population_size, int tournament_size, 
			   double mutation_rate, double crossover_rate, 
			   int n_concurrent_threads = 4);

	void next_generation();
	int tournament_selection();
	int pareto_ranked_tournament_selection();
	
	//MOEA sorting
	void non_dominated_sort();
	
	//Crowding distance calculations
	void calculate_crowding_distances();
	void calculate_crowding_distance_edge_contribution(const std::vector<int>& sorted_by_object_function);
	void calculate_crowding_distance_deviation_contribution(const std::vector<int>& sorted_by_object_function);

	//Crowding distance sorting
	void sort_pareto_fronts();
	void sort_front_by_crowding_distance(int front);
	void sort_front_by_edge_value(int front, std::vector<int>& sorted_by_edge_value);
	void sort_front_by_overall_deviation(int front, std::vector<int>& sorted_by_overall_deviation);
	
	void create_children();
	void create_phenotypes();
	Genotype get_genotype(int i) { return population[i]; }

	void test();
	void test2() { std::cout << population[0]; }

};
