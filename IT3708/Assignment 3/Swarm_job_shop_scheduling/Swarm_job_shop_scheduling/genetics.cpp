//#include "data.h"
#include "genetics.h"
#include <vector>
#include <cstdlib>
#include <algorithm>

//Chromosome
Chromosome::Chromosome() {

}

//okay
Chromosome::Chromosome(char* str): Chromosome() {
	for (int i = 0; str[i] != '\0'; i++) {
		chromosome_string[i] = str[i] - '0'; //translate to indices
	}
}



//OTTOREP
OttoRep::OttoRep(char* str): OttoRep(){
	for (int i = 0; str[i] != '\0'; i++) {
		chromosome_string[i] = str[i] - '0'; //translate to indices
	}
}

OttoRep::OttoRep(const OttoRep& chromosome): OttoRep() {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		chromosome_string[i] = chromosome.chromosome_string[i];
	}
}

void OttoRep::_mutate_swap1() {
	unsigned int m1 = rand() % data.N_MACHINES;
	unsigned int m2 = rand() % data.N_MACHINES;
	unsigned int job = rand() % data.N_JOBS;

	unsigned int m1_base = m1 * data.N_JOBS;
	unsigned int m2_base = m2 * data.N_JOBS;

	unsigned int temp = chromosome_string[m1_base + job];
	chromosome_string[m1_base + job] = chromosome_string[m2_base + job];
	chromosome_string[m2_base + job] = temp;
}

void OttoRep::_mutate_swap2() {
	_mutate_swap1();
	_mutate_swap1();
}



//Picks a machine and moves a job to another location
void OttoRep::_mutate_insert1() {
	unsigned int m = rand() % data.N_MACHINES;
	unsigned int j1 = rand() % data.N_JOBS;
	unsigned int j2 = rand() % data.N_JOBS;
	unsigned int j_max = std::max(j1, j2);
	unsigned int j_min = std::min(j1, j2);

	unsigned int m_base = m*data.N_JOBS;
	unsigned int inserted_job = chromosome_string[m_base + j_min];

	for (int i = j_min + m_base; i < j_max + m_base; i++) {
		chromosome_string[i] = chromosome_string[i + 1];

		if (inserted_job > chromosome_string[i]) {
			inserted_job--;
		}
		else {
			chromosome_string[i]++;
		}
	}

	chromosome_string[m_base + j_max] = inserted_job;
}

//Runs insert 
void OttoRep::_mutate_insert2() {
	_mutate_insert1();
	_mutate_insert1();
}

void OttoRep::_mutate_flip1() {
	unsigned int machine = rand() % data.N_MACHINES;
	unsigned int job = rand() % data.N_JOBS;

	chromosome_string[machine*data.N_JOBS + job] = rand() % (data.N_JOBS - job);
}

void OttoRep::_mutate_flip2() {
	_mutate_flip1();
	_mutate_flip1();
}

void OttoRep::_mutate_swap_machines() {
	unsigned int m1 = rand() % data.N_MACHINES;
	unsigned int m2 = rand() % data.N_MACHINES;

	unsigned int m1_base = data.N_JOBS*m1;
	unsigned int m2_base = data.N_JOBS*m2;

	for (int job = 0; job < data.N_JOBS; job++) {
		unsigned int temp = chromosome_string[m1_base + job];
		chromosome_string[m1_base + job] = chromosome_string[m2_base + job];
		chromosome_string[m2_base + job] = temp;
	}
}

void OttoRep::mutate(MUTATION_OPERATIONS mutation) {
	switch (mutation) {
	case (SWAP1): 
		_mutate_swap1();
		break;
	case (SWAP2):
		_mutate_swap2();
		break;
	case (INSERT1):
		_mutate_insert1();
		break;
	case (INSERT2):
		_mutate_insert2();
		break;
	case (MSWAP):
		_mutate_swap_machines();
		break;

	case (FLIP1):
		_mutate_flip1();
		break;	
	case (FLIP2):
		_mutate_flip2();
		break;
	}
}

unsigned int OttoRep::_relative_to_absolute_job(unsigned int machine, unsigned int job) {
	unsigned int machine_base = machine*data.N_JOBS;
	unsigned int absolute = chromosome_string[machine_base + job];

	for (int j = job - 1; j >= 0; j--) {
		if (chromosome_string[machine_base + j] <= absolute) {
			absolute++;
		}
	}

	return absolute;
}

unsigned int OttoRep::_absolute_to_relative_job(unsigned int absolute, unsigned int machine, unsigned int job) {
	unsigned int machine_base = machine*data.N_JOBS;
	unsigned int relative = absolute;
	
	for (int i = machine_base; i < machine_base + job; i++) {
		if (relative > chromosome_string[i]) {
			relative--;
		}
	}

	return relative;
}

void OttoRep::convert_to_phenotype(Phenotype& phenotype) {
	//For each machine
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		
		//For each job of the machine
		for (int i = 0; i < data.N_JOBS; i++) {

			//Iterate through all earlier jobs to translate 
			unsigned int job = _relative_to_absolute_job(machine, i);
			phenotype.add_job(machine, i, job);
		}
	}
}

bool OttoRep::operator == (const OttoRep &rhs) {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		if (this->chromosome_string[i] != rhs.chromosome_string[i]) {
			return false;
		}
	}

	return true;
}

bool OttoRep::operator == (const OttoRep &rhs) const {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		if (this->chromosome_string[i] != rhs.chromosome_string[i]) {
			return false;
		}
	}

	return true;
}

OttoRep OttoRep::operator =(const OttoRep &rhs) {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		chromosome_string[i] = rhs.chromosome_string[i];
	}

	return *this;
}

void OttoRep::random_initialization() {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		unsigned int base = machine*data.N_JOBS;
		for (int job = 0; job < data.N_JOBS; job++) {
			chromosome_string[base + job] = rand() % (data.N_JOBS - job);
		}
	}
}

std::ostream& operator << (std::ostream& out, const OttoRep& chromosome) {
	out << chromosome.chromosome_string[0];

	for (int i = 1; i < data.N_JOBS*data.N_MACHINES; i++) {
		out << ", " << chromosome.chromosome_string[i];
	}

	return out;
}





//Phenotype
Phenotype::Phenotype() {
	work_order = new unsigned int*[data.N_MACHINES];
	fitness = 0;

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		work_order[machine] = new unsigned int[data.N_JOBS];
	}
}

Phenotype::Phenotype(const Phenotype& rhs): Phenotype() {
	this->fitness = rhs.fitness;

	for (int i = 0; i < data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			this->work_order[i][j] = rhs.work_order[i][j];
		}
	}
}

Phenotype::~Phenotype() {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		delete[] work_order[machine];
	}

	delete[] work_order;
}

void Phenotype::calculate_fitness() {
	PhenotypeScheduler scheduler;
	scheduler.assign_jobs(work_order);
	deadlock_happened = false;
	fitness = 0;
	
	while (scheduler.jobs_left) {

		unsigned int time_step = scheduler.lowest_remaining_execution_time();
		if (time_step == 0) {
			//std::cout << "Deadlock detected!\n";
			scheduler.deadlock_handler(work_order);
			time_step = scheduler.lowest_remaining_execution_time();
			deadlock_happened = true;
			
		}

		fitness += time_step;
		scheduler.execution_step(time_step, work_order, fitness);

		scheduler.assign_jobs(work_order);
	}

	//Rerun fitness calculation
	if (deadlock_happened) {
		calculate_fitness();
	}
	
}

Phenotype Phenotype::operator =(Phenotype rhs) {
	this->fitness = rhs.fitness;

	for (int i = 0; i < data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			this->work_order[i][j] = rhs.work_order[i][j];
		}
	}

	return *this;
}

void Phenotype::convert_to_genotype(OttoRep& chromosome) {

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		unsigned int base = machine*data.N_JOBS;

		for (int job = 0; job < data.N_JOBS; job++) {
			
			//Iterate backwards and calculate genotype value
			unsigned int genotype_value = work_order[machine][job];
			for (int i = job-1; i >= 0; i--) {
				if (work_order[machine][i] <= genotype_value) {
					genotype_value--;
				}
			}
			chromosome[base + job] = genotype_value;
		}
	}
}

std::ostream& operator << (std::ostream& out, const Phenotype& schedule) {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		for (int job = 0; job < data.N_JOBS; job++) {
			out << schedule.work_order[machine][job] << "\t";	
		}
		out << std::endl;
	}

	return out;
}



//PhenotypeScheduler

PhenotypeScheduler::PhenotypeScheduler() {
	remaining_execution_time.resize(data.N_MACHINES, 0);
	machine_progress.resize(data.N_MACHINES, 0);
	job_progress.resize(data.N_JOBS, 0);
	required_machine.resize(data.N_JOBS, 0);

	jobs_left = data.N_JOBS;
	total_execution_time = 0;

	//Set the first machine required for each job.
	for (int job = 0; job < data.N_JOBS; job++) {
		unsigned int beginning_machine = data.work_order[job][0];
		required_machine[job] = beginning_machine;
	}
}

unsigned int PhenotypeScheduler::lowest_remaining_execution_time() {
	unsigned int lowest_value = 999999;

	for (int i = 0; i < data.N_MACHINES; i++) {
		if (lowest_value > remaining_execution_time[i] && remaining_execution_time[i] > 0) {
			lowest_value = remaining_execution_time[i];
		}
	}

	if (lowest_value == 999999) {
		return 0;
	}


	return lowest_value;
}

void PhenotypeScheduler::deadlock_handler(unsigned int** work_order) {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		for (int job = 0; job < data.N_JOBS; job++) {
			
			if (required_machine[job] == machine && machine_progress[machine] != data.N_JOBS) {

//				std::cout << work_order[machine][0] << ", " << work_order[machine][1] << ", " << work_order[machine][2] << std::endl;
				
				//Find out when the machine was supposed to do the job
				unsigned int current_index = machine_progress[machine];
				for (int i = current_index; i < data.N_JOBS; i++) {

					if (work_order[machine][i] == job) {
						//Swap the required job with the one that is stuck
						unsigned int temp = work_order[machine][current_index];
						work_order[machine][current_index] = work_order[machine][i];
						work_order[machine][i] = temp;
						remaining_execution_time[machine] = data.execution_time[job][job_progress[job]];

						return;
					}

				}
			}

		}
	}
}

void PhenotypeScheduler::execution_step(unsigned int time_step, unsigned int** work_order, unsigned int fitness) {
	//std::cout << data << std::endl;

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		remaining_execution_time[machine] -= time_step;

		//Machine finished executing the current job
		if (remaining_execution_time[machine] == 0 && machine_progress[machine] != data.N_JOBS) {
			unsigned int finished_job = work_order[machine][machine_progress[machine]];
			//std::cout << fitness << ": Machine " << machine << ", Finished executing job " << finished_job << std::endl;

			machine_progress[machine]++;	//Update progress for the machine
			job_progress[finished_job]++;	//Update progress for the job
			

			//std::cout << data;
			required_machine[finished_job] = data.work_order[finished_job][job_progress[finished_job]]; //jobs are one indexed

			//Every job worked on. Machine is finished.
			if (machine_progress[machine] == data.N_JOBS) {
				jobs_left--;
			}

		}

	}
}

void PhenotypeScheduler::assign_jobs(unsigned int **work_order) {
	for (unsigned int machine = 0; machine < data.N_MACHINES; machine++) {

		//Machine is done working. Don't assign anymore jobs to this one
		if (machine_progress[machine] == data.N_JOBS) {
			continue;
		}

		unsigned int next_job = work_order[machine][machine_progress[machine]];

		//The machine is cleared to work on the job
		if (required_machine[next_job] == machine) {

			//The machine has finished executing
			//Assign the new job
			if (remaining_execution_time[machine] <= 0) {
				//std::cout << "Machine " << machine << ": Assigned to job " << next_job << std::endl;
				remaining_execution_time[machine] = data.execution_time[next_job][job_progress[next_job]];
			}
		}

		//The machine has to wait until another job is finished.
		else {
			//std::cout << "Machine " << machine << ": Waiting to run job " << next_job << std::endl;
			remaining_execution_time[machine] = -1;
		}

	}

}


/*
//Graph based approach
void Phenotype::calculate_fitness() {
	unsigned int adjacency_list** = new unsigned int

}
*/
/*
void OttoRep::n_point_crossover(const Chromosome &p2, Chromosome& c1, Chromosome& c2, unsigned int n_crossover_points) {
unsigned int cross2 = rand() % (INDIRECT_CHROMOSOME_LENGTH - 1) + 1;
unsigned int cross1 = rand() % cross2;
unsigned int prev_cross_end = 0;

//Special case with a single crossover point
if (n_crossover_points == 1) {

for (unsigned int i = prev_cross_end; i < cross1; i++) {
c1[i] = (*this)[i];
c2[i] = p2[i];
}

for (unsigned int i = cross1; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
c2[i] = (*this)[i];
c1[i] = p2[i];
}
return;
}


//Create children through a series of crossover points
for (int i = 1; i < n_crossover_points && cross1 != (INDIRECT_CHROMOSOME_LENGTH-1); i++) {

for (unsigned int i = prev_cross_end; i < cross1; i++) {
c1[i] = (*this)[i];
c2[i] = p2[i];
}

for (unsigned int i = cross1; i < cross2; i++) {
c2[i] = (*this)[i];
c1[i] = p2[i];
}
prev_cross_end = cross2;
cross2 = rand() % (INDIRECT_CHROMOSOME_LENGTH - prev_cross_end + 1) + prev_cross_end;
cross1 = rand() % (cross2 - prev_cross_end + 1) + prev_cross_end;
}

//Fill the last part of the chromosomes
if (n_crossover_points % 2 == 0) {
for (int i = prev_cross_end; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
c1[i] = (*this)[i];
c2[i] = p2[i];
}
}
else {
for (int i = prev_cross_end; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
c2[i] = (*this)[i];
c1[i] = p2[i];
}
}
}
*/