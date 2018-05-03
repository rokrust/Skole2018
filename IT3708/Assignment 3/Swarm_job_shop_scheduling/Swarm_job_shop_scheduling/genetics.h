#pragma once
#include "config.h"
#include "data.h"
#include <iostream>
#include <vector>

class Phenotype {
private:
	int fitness;
	unsigned int** work_order;

public:
	Phenotype();
	~Phenotype();
	void calculate_fitness();

	void add_job(unsigned int machine, unsigned int at_job, unsigned int job) { work_order[machine][at_job] = job; }
	int get_fitness() { return fitness; }
};

class PhenotypeScheduler {
private:
	bool no_job_scheduled;
	unsigned int jobs_left; //Amount of unfinished jobs
	unsigned int total_execution_time;

	std::vector<int> remaining_execution_time; //Time until each machine is done with its current job
	std::vector<unsigned int> machine_progress; //How many jobs each machine has been working on
	std::vector<unsigned int> job_progress; //How many machines have worked on each job
	std::vector<unsigned int> required_machine; //The next machine required to work on each job

public:
	PhenotypeScheduler();

	unsigned int lowest_remaining_execution_time();
	void execution_step(unsigned int step_time, unsigned int** work_order, unsigned int fitness);
	void assign_jobs(unsigned int** work_order);
	void deadlock_handler();

	friend Phenotype;
};


class Chromosome {
private:


public:
	Chromosome();
	Chromosome(char* str);

	virtual void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points) =0;
	virtual void convert_to_phenotype(Phenotype& phenotype) = 0;

	const unsigned int& operator [](unsigned int i) const { return chromosome_string[i]; }
	unsigned int& operator [](unsigned int i) { return chromosome_string[i]; }
	const unsigned int& operator [](int i) const { return chromosome_string[i]; }
	unsigned int& operator [](int i) { return chromosome_string[i]; }


protected:
	unsigned int* chromosome_string;
	unsigned int chromosome_length;
};

enum MUTATION_OPERATIONS { SWAP1, SWAP2, INSERT1, INSERT2 };

class OttoRep : public Chromosome{
private:
	void _mutate_swap1();
	void _mutate_swap2();
	void _mutate_insert1();
	void _mutate_insert2();

	unsigned int _relative_to_absolute_job(unsigned int machine, unsigned int job);
	unsigned int _absolute_to_relative_job(unsigned int absolute, unsigned int machine, unsigned int job);
public:
	OttoRep() { chromosome_length = INDIRECT_CHROMOSOME_LENGTH;  chromosome_string = new unsigned int[INDIRECT_CHROMOSOME_LENGTH]; }
	OttoRep(char* str);
	~OttoRep() { delete[] chromosome_string; }

	void mutate(MUTATION_OPERATIONS mutation);

	void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points = 1);
	void convert_to_phenotype(Phenotype& phenotype);


	bool operator == (const OttoRep &rhs);
	bool operator == (const OttoRep &rhs) const;
	friend std::ostream& operator << (std::ostream& out, const OttoRep& chromosome);

	void test() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				std::cout << chromosome_string[i * 3 + j] << '\t';
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				unsigned int absolute = _relative_to_absolute_job(i, j);
				std::cout <<  _absolute_to_relative_job(absolute, i, j) << '\t';
			}
			std::cout << std::endl;
		}
		
	}
};