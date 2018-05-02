#pragma once
#include "config.h"
#include "data.h"
#include <iostream>
#include <vector>

class Phenotype {
private:
	int fitness;
	unsigned int** work_order;
	
	unsigned int _lowest_remaining_execution_time(std::vector<unsigned int> remaining_execution_time);
	void _deadlock_handler();

public:
	Phenotype();
	~Phenotype();
	void calculate_fitness();

	void add_job(unsigned int machine, unsigned int at_job, unsigned int job) { work_order[machine][at_job] = job; }
	int get_fitness() { return fitness; }
};


class Chromosome {
private:


public:
	Chromosome();
	Chromosome(char* str);

	virtual void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points) =0;
	virtual void mutate() =0;

	const unsigned int& operator [](unsigned int i) const { return chromosome_string[i]; }
	unsigned int& operator [](unsigned int i) { return chromosome_string[i]; }
	const unsigned int& operator [](int i) const { return chromosome_string[i]; }
	unsigned int& operator [](int i) { return chromosome_string[i]; }

protected:
	unsigned int* chromosome_string;
	unsigned int chromosome_length;
};

class OttoRep : public Chromosome{
private:

public:
	OttoRep() { chromosome_length = INDIRECT_CHROMOSOME_LENGTH;  chromosome_string = new unsigned int[INDIRECT_CHROMOSOME_LENGTH]; }
	OttoRep(char* str);
	~OttoRep() { delete chromosome_string; }

	void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points = 1);
	void mutate();
	void convert_to_phenotype(Phenotype& phenotype);


	bool operator == (const OttoRep &rhs);
	bool operator == (const OttoRep &rhs) const;
	friend std::ostream& operator << (std::ostream& out, const OttoRep& chromosome);
};