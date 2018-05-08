#pragma once
#include "config.h"
#include "genetics.h"

class Path; class Map; class Ant; class AntColony;

//The path between two nodes
class Path {
private:
	double _pheromone_strength;
	void _saturate_pheromone_strength();

public:
	Path();
	double get_pheromone_strength() const { return _pheromone_strength; }
	Path operator*=(double w);
	Path operator+(double w);
	Path operator+=(double w);
	Path operator=(double w);
};

class Map {
private:
	Path*** pheromone_trail;
	Path* start_node;
	
	void _allocate_data();
	int _find_next_path(int layer, int from, std::vector<int> excluded_paths);
	double _get_heuristic_weight(int layer, int from, int to);

public:
	Map();
	Map(const Map& map);
	~Map();

	void find_new_trail(Ant& ant);
	void evaporate_pheromone_trail();
	void update_pheromone_trail(const Ant& ant);
	
	double get_path_pheromone_strength(int layer, int from, int to);

	Map operator=(const Map& map);
	friend std::ostream& operator<<(std::ostream& out, const Map& map);
};

class Ant {
private:
	CHROMOSOME_TYPE solution;
	Phenotype schedule;

public:
	Ant();
	Ant(const Ant& ant);
	void mutate(MUTATION_OPERATIONS mutation) { solution.mutate(mutation); }
	void local_search();
	void search();

	int get_job(int i) const { return solution[i]; }
	void set_job(int i, int job) { solution[i] = job; }
	double get_pheromone_strength() const { return ACO_FITNESS_PHEROMONE_CONTRIBUTION / double(schedule.get_fitness()); }
	int get_fitness() { return schedule.get_fitness(); }
	void calculate_pheromone_contribution();

	void test() const { std::cout << solution; }
	bool operator == (Ant rhs) { return get_pheromone_strength() == rhs.get_pheromone_strength(); }
	bool operator >= (Ant rhs) { return get_pheromone_strength() >= rhs.get_pheromone_strength(); }
	bool operator <= (Ant rhs) { return get_pheromone_strength() <= rhs.get_pheromone_strength(); }
	bool operator > (Ant rhs) { return get_pheromone_strength() > rhs.get_pheromone_strength(); }
	bool operator < (Ant rhs) { return get_pheromone_strength() < rhs.get_pheromone_strength(); }
	Ant operator = (const Ant& rhs);

};

class AntColony {
private:
	Map map;
	std::vector<Ant> population;
	std::vector<Ant> elite_ants;


	Ant* _tournament_selection();
public:
	AntColony();
	AntColony(const AntColony& colony);

	void build_trails();
	void update_trails();
	void run_optimization(int optimal_value = 0);
	AntColony operator=(const AntColony& colony);
};
