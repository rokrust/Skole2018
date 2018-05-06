#pragma once
#include "genetics.h"
#include<vector>

class FoodSource {
private:
	unsigned int time_to_live;

	OttoRep solution;
	Phenotype schedule;

public:
	FoodSource();
	FoodSource(char* representation);
	FoodSource(const FoodSource& food_source) : solution(food_source.solution), 
												schedule(food_source.schedule), 
												time_to_live(food_source.time_to_live) { ; }

	void create_scout_bee();

	void local_search();
	bool search_operation(MUTATION_OPERATIONS operation);

	unsigned int get_remaining_time_to_live() { return time_to_live; }
	unsigned int get_nectar_amount() { return schedule.get_fitness(); }

	FoodSource operator = (FoodSource rhs);
	bool operator >= (FoodSource rhs) { return schedule.get_fitness() >= rhs.schedule.get_fitness(); }
	bool operator <= (FoodSource rhs) { return schedule.get_fitness() <= rhs.schedule.get_fitness(); }
	bool operator > (FoodSource rhs) { return schedule.get_fitness() > rhs.schedule.get_fitness(); }
	bool operator < (FoodSource rhs) { return schedule.get_fitness() < rhs.schedule.get_fitness(); }

	//Operators
	friend std::ostream& operator << (std::ostream& out, const FoodSource& food_source);
};

class Hive {
private:
	std::vector<FoodSource> employed_bees;
	std::vector<FoodSource*> old_bee;
	FoodSource elite_bee;

	std::vector<MUTATION_OPERATIONS> adaptive_list;
	std::vector<MUTATION_OPERATIONS> successful_mutations;

	//Does not keep the old list when winning list is empty.
	void _refill_adaptive_list();
	void _run_adaptive_list_operation(FoodSource* bee);
	void _search_for_new_sources(FoodSource* bee);
	FoodSource* _tournament_selection();

public:
	Hive();

	void employed_bee_phase();
	void onlooker_bee_phase();
	void scout_bee_phase();
	void run_optimization(unsigned int optimal_value=0);
};