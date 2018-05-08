#pragma once
#include "genetics.h"
#include<vector>

class Bee {
private:
	int time_to_live;

	CHROMOSOME_TYPE solution;
	Phenotype schedule;

public:
	Bee();
	Bee(char* representation);
	Bee(const Bee& bee) : solution(bee.solution), schedule(bee.schedule), time_to_live(bee.time_to_live) { ; }

	void create_scout_bee();
	void local_search();
	bool search_operation(MUTATION_OPERATIONS operation);

	int get_remaining_time_to_live() { return time_to_live; }
	int get_nectar_amount() { return schedule.get_fitness(); }

	Bee operator = (const Bee& rhs);
	bool operator >= (Bee rhs) { return schedule.get_fitness() >= rhs.schedule.get_fitness(); }
	bool operator <= (Bee rhs) { return schedule.get_fitness() <= rhs.schedule.get_fitness(); }
	bool operator > (Bee rhs) { return schedule.get_fitness() > rhs.schedule.get_fitness(); }
	bool operator < (Bee rhs) { return schedule.get_fitness() < rhs.schedule.get_fitness(); }

	void print() { std::cout << schedule << std::endl; }

	//Operators
	friend std::ostream& operator << (std::ostream& out, const Bee& bee);
};

class Hive {
private:
	std::vector<Bee> employed_bees;
	std::vector<Bee*> old_bee;
	Bee elite_bee;

	std::vector<MUTATION_OPERATIONS> adaptive_list;
	std::vector<MUTATION_OPERATIONS> successful_mutations;

	//Does not keep the old list when winning list is empty.
	void _refill_adaptive_list();
	void _run_adaptive_list_operation(Bee* bee);
	void _search_for_new_sources(Bee* bee);
	Bee* _tournament_selection();

public:
	Hive();

	void employed_bee_phase();
	void onlooker_bee_phase();
	void scout_bee_phase();
	void run_optimization(int optimal_value=0);
};