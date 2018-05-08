#pragma once
#include "config.h"
#include "data.h"
#include <iostream>
#include <vector>

class Phenotype; 
class PhenotypeScheduler;
class Chromosome;
class OttoRep;
class NewRep;

class Phenotype {
private:
	int fitness;
	int** work_order;
	bool deadlock_happened;

public:
	Phenotype();
	Phenotype(const Phenotype& phenotype);
	~Phenotype();
	void calculate_fitness();

	bool is_initialized() { return fitness != 0; }
	void add_job(int machine, int at_job, int job) { work_order[machine][at_job] = job; }
	int get_fitness() { return fitness; }
	int get_fitness() const { return fitness; }
	bool genotype_mismatch() { return deadlock_happened; }
	void convert_to_genotype(CHROMOSOME_TYPE& genotype);

	bool operator >=(Phenotype rhs) { return this->fitness >= rhs.fitness; }
	bool operator <=(Phenotype rhs) { return this->fitness <= rhs.fitness; }
	bool operator >(Phenotype rhs) { return this->fitness > rhs.fitness; }
	bool operator <(Phenotype rhs) { return this->fitness < rhs.fitness; }
	Phenotype operator =(Phenotype rhs);

	friend std::ostream& operator << (std::ostream& out, const Phenotype& schedule);
};

class PhenotypeScheduler {
private:
	
	int jobs_left; //Amount of unfinished jobs
	int total_execution_time;

	std::vector<int> remaining_execution_time; //Time until each machine is done with its current job
	std::vector<int> machine_progress; //How many jobs each machine has been working on
	std::vector<int> job_progress; //How many machines have worked on each job
	std::vector<int> required_machine; //The next machine required to work on each job

public:
	PhenotypeScheduler();

	int lowest_remaining_execution_time();
	void execution_step(int step_time, int** work_order, int fitness);
	void assign_jobs(int** work_order);
	void deadlock_handler(int** work_order);

	friend Phenotype;
};

//enum MUTATION_OPERATIONS { SWAP1, SWAP2, INSERT1, INSERT2, MSWAP, FLIP1, FLIP2, N_MUTATIONS };
enum MUTATION_OPERATIONS { SWAP1, SWAP2, INSERT1, INSERT2, N_MUTATIONS, FLIP1, FLIP2, MSWAP};

class Chromosome {
private:


public:
	Chromosome();
	Chromosome(char* str);

	//virtual void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const int n_crossover_points) =0;
	virtual void convert_to_phenotype(Phenotype& phenotype) = 0;
	virtual void mutate(MUTATION_OPERATIONS mutation) = 0;

	const int& operator [](int i) const { return chromosome_string[i]; }
	int& operator [](int i) { return chromosome_string[i]; }

protected:
	int* chromosome_string;
};

class NewRep : public Chromosome {
private:
	void _mutate_swap1();
	void _mutate_swap2();
	void _mutate_insert1();
	void _mutate_insert2();

public:
	NewRep() { chromosome_string = new int[data.N_JOBS*data.N_MACHINES]; }
	NewRep(char* str);
	NewRep(const NewRep& chromosome);
	~NewRep() { delete[] chromosome_string; }
	
	void random_initialization();

	//Virtual functions
	void convert_to_phenotype(Phenotype& phenotype);
	void mutate(MUTATION_OPERATIONS mutation);

	NewRep operator =(const NewRep &rhs);
	friend std::ostream& operator << (std::ostream& out, const NewRep& chromosome);
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


	int _relative_to_absolute_job(int machine, int job);
	int _absolute_to_relative_job(int absolute, int machine, int job);
public:
	OttoRep() { chromosome_string = new int[data.N_JOBS*data.N_MACHINES];}
	OttoRep(char* str);
	OttoRep(const OttoRep& chromosome);
	~OttoRep() { delete[] chromosome_string; }

	void random_initialization();

	void mutate(MUTATION_OPERATIONS mutation);

	//void n_point_crossover(const Chromosome &rhs_parent, Chromosome& offspring1, Chromosome& offspring2, const int n_crossover_points = 1);
	void convert_to_phenotype(Phenotype& phenotype);


	bool operator == (const OttoRep &rhs);
	bool operator == (const OttoRep &rhs) const;
	OttoRep operator =(const OttoRep &rhs);
	friend std::ostream& operator << (std::ostream& out, const OttoRep& chromosome);

};