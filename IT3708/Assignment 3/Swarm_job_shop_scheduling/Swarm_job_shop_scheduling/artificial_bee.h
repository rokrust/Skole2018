#pragma once
#include "genetics.h"
#include<vector>

class FoodSource {
private:
	unsigned int time_to_live;
	unsigned int nectar_amount;
	Chromosome *solution;
	Phenotype phenotype;
	std::vector<MUTATION_OPERATIONS> adaptive_list;

public:
	FoodSource();
	FoodSource(char* representation);

	void neighboring_solution();
};