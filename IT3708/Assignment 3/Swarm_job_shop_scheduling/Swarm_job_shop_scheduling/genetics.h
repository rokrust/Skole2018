#pragma once
#include "config.h"
#include "data.h"
#include <iostream>
#include <vector>

class Phenotype; 
class PhenotypeScheduler;
class Chromosome;
class OttoRep;

class Phenotype {
private:
	int fitness;
	unsigned int** work_order;
	bool deadlock_happened;

public:
	Phenotype();
	Phenotype(const Phenotype& phenotype);
	~Phenotype();
	void calculate_fitness();

	bool is_initialized() { return fitness != 0; }
	void add_job(unsigned int machine, unsigned int at_job, unsigned int job) { work_order[machine][at_job] = job; }
	int get_fitness() { return fitness; }
	bool genotype_mismatch() { return deadlock_happened; }
	void convert_to_genotype(OttoRep& genotype);

	bool operator >=(Phenotype rhs) { return this->fitness >= rhs.fitness; }
	bool operator <=(Phenotype rhs) { return this->fitness <= rhs.fitness; }
	bool operator >(Phenotype rhs) { return this->fitness > rhs.fitness; }
	bool operator <(Phenotype rhs) { return this->fitness < rhs.fitness; }
	Phenotype operator =(Phenotype rhs);

	friend std::ostream& operator << (std::ostream& out, const Phenotype& schedule);
};

class PhenotypeScheduler {
private:
	
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
	void deadlock_handler(unsigned int** work_order);

	friend Phenotype;
};

//enum MUTATION_OPERATIONS { SWAP1, SWAP2, INSERT1, INSERT2, MSWAP, FLIP1, FLIP2, N_MUTATIONS };
enum MUTATION_OPERATIONS { SWAP1, SWAP2, INSERT1, INSERT2, N_MUTATIONS, FLIP1, FLIP2, MSWAP};

class Chromosome {
private:


public:
	Chromosome();
	Chromosome(char* str);

	//virtual void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points) =0;
	virtual void convert_to_phenotype(Phenotype& phenotype) = 0;
	virtual void mutate(MUTATION_OPERATIONS mutation) = 0;

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
	void _mutate_swap1();
	void _mutate_swap2();
	void _mutate_insert1();
	void _mutate_insert2();
	void _mutate_flip1();
	void _mutate_flip2();
	void _mutate_swap_machines();


	unsigned int _relative_to_absolute_job(unsigned int machine, unsigned int job);
	unsigned int _absolute_to_relative_job(unsigned int absolute, unsigned int machine, unsigned int job);
public:
	OttoRep() { chromosome_string = new unsigned int[data.N_JOBS*data.N_MACHINES]; random_initialization(); }
	OttoRep(char* str);
	OttoRep(const OttoRep& chromosome);
	~OttoRep() { delete[] chromosome_string; }

	void random_initialization();

	void mutate(MUTATION_OPERATIONS mutation);

	//void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const unsigned int n_crossover_points = 1);
	void convert_to_phenotype(Phenotype& phenotype);


	bool operator == (const OttoRep &rhs);
	bool operator == (const OttoRep &rhs) const;
	OttoRep operator =(const OttoRep &rhs);
	friend std::ostream& operator << (std::ostream& out, const OttoRep& chromosome);

};