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
	int m1 = rand() % data.N_MACHINES;
	int m2 = rand() % data.N_MACHINES;
	int job = rand() % data.N_JOBS;

	int m1_base = m1 * data.N_JOBS;
	int m2_base = m2 * data.N_JOBS;

	int temp = chromosome_string[m1_base + job];
	chromosome_string[m1_base + job] = chromosome_string[m2_base + job];
	chromosome_string[m2_base + job] = temp;
}

void OttoRep::_mutate_swap2() {
	_mutate_swap1();
	_mutate_swap1();
}



//Picks a machine and moves a job to another location
void OttoRep::_mutate_insert1() {
	int m = rand() % data.N_MACHINES;
	int j1 = rand() % data.N_JOBS;
	int j2 = rand() % data.N_JOBS;
	int j_max = std::max(j1, j2);
	int j_min = std::min(j1, j2);

	int m_base = m*data.N_JOBS;
	int inserted_job = chromosome_string[m_base + j_min];

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
	int machine = rand() % data.N_MACHINES;
	int job = rand() % data.N_JOBS;

	chromosome_string[machine*data.N_JOBS + job] = rand() % (data.N_JOBS - job);
}

void OttoRep::_mutate_flip2() {
	_mutate_flip1();
	_mutate_flip1();
}

void OttoRep::_mutate_swap_machines() {
	int m1 = rand() % data.N_MACHINES;
	int m2 = rand() % data.N_MACHINES;

	int m1_base = data.N_JOBS*m1;
	int m2_base = data.N_JOBS*m2;

	for (int job = 0; job < data.N_JOBS; job++) {
		int temp = chromosome_string[m1_base + job];
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

int OttoRep::_relative_to_absolute_job(int machine, int job) {
	int machine_base = machine*data.N_JOBS;
	int absolute = chromosome_string[machine_base + job];

	for (int j = job - 1; j >= 0; j--) {
		if (chromosome_string[machine_base + j] <= absolute) {
			absolute++;
		}
	}

	return absolute;
}

int OttoRep::_absolute_to_relative_job(int absolute, int machine, int job) {
	int machine_base = machine*data.N_JOBS;
	int relative = absolute;
	
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
			int job = _relative_to_absolute_job(machine, i);
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
		int base = machine*data.N_JOBS;
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
	work_order = new int*[data.N_MACHINES];
	fitness = 0;

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		work_order[machine] = new int[data.N_JOBS];
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


		int time_step = scheduler.lowest_remaining_execution_time();
		if (time_step == 0) {
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

void Phenotype::convert_to_genotype(CHROMOSOME_TYPE& chromosome) {

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		int base = machine*data.N_JOBS;

		for (int job = 0; job < data.N_JOBS; job++) {
			
			//Iterate backwards and calculate genotype value
			int genotype_value = work_order[machine][job];
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

	jobs_left = data.N_MACHINES;
	total_execution_time = 0;

	//Set the first machine required for each job.
	for (int job = 0; job < data.N_JOBS; job++) {
		int beginning_machine = data.work_order[job][0];
		required_machine[job] = beginning_machine;
	}
}

int PhenotypeScheduler::lowest_remaining_execution_time() {
	int lowest_value = 999999;

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

void PhenotypeScheduler::deadlock_handler(int** work_order) {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		for (int job = 0; job < data.N_JOBS; job++) {
			
			if (required_machine[job] == machine && machine_progress[machine] != data.N_JOBS) {

//				std::cout << work_order[machine][0] << ", " << work_order[machine][1] << ", " << work_order[machine][2] << std::endl;
				
				//Find out when the machine was supposed to do the job
				int current_index = machine_progress[machine];
				for (int i = current_index; i < data.N_JOBS; i++) {

					if (work_order[machine][i] == job) {
						//Swap the required job with the one that is stuck
						int temp = work_order[machine][current_index];
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

void PhenotypeScheduler::execution_step(int time_step, int** work_order, int fitness) {
	//std::cout << data << std::endl;

	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		remaining_execution_time[machine] -= time_step;

		//Machine finished executing the current job
		if (remaining_execution_time[machine] == 0 && machine_progress[machine] != data.N_JOBS) {
			int finished_job = work_order[machine][machine_progress[machine]];
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

void PhenotypeScheduler::assign_jobs(int **work_order) {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {

		//Machine is done working. Don't assign anymore jobs to this one
		if (machine_progress[machine] == data.N_JOBS) {
			continue;
		}

		int next_job = work_order[machine][machine_progress[machine]];

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




//NewRep
NewRep::NewRep(char* str) : NewRep() {
	for (int i = 0; str[i] != '\0'; i++) {
		chromosome_string[i] = str[i] - '0'; //translate to indices
	}
}

NewRep::NewRep(const NewRep& chromosome) : NewRep() {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		chromosome_string[i] = chromosome.chromosome_string[i];
	}
}

void NewRep::convert_to_phenotype(Phenotype& phenotype) {
	std::vector<int> job_progress(data.N_JOBS, 0);
	std::vector<int> machine_progress(data.N_MACHINES, 0);

	//Iterate through chromosome string
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		int job = chromosome_string[i];

 		int required_machine = data.work_order[job][job_progress[job]];
		phenotype.add_job(required_machine, machine_progress[required_machine], job);
		machine_progress[required_machine]++;
		job_progress[job]++;
	}
}

void NewRep::_mutate_swap1() {
	int i = rand() % data.N_JOBS*data.N_MACHINES;
	int j = rand() % data.N_JOBS*data.N_MACHINES;

	int temp = chromosome_string[i];
	chromosome_string[i] = chromosome_string[j];
	chromosome_string[j] = temp;
}

void NewRep::_mutate_swap2() {
	_mutate_swap1();
	_mutate_swap1();
}

void NewRep::_mutate_insert1() {
	int i = rand() % data.N_JOBS*data.N_MACHINES;
	int j = rand() % data.N_JOBS*data.N_MACHINES;

	int job = chromosome_string[i];

	if (i < j) {
		for (int k = i; k < j; k++) {
			chromosome_string[k] = chromosome_string[k + 1];
		}
	}
	else {
		for (int k = i; k >= j; k--) {
			chromosome_string[k] = chromosome_string[k - 1];
		}
	}

	chromosome_string[j] = job;
}

void NewRep::_mutate_insert2() {
	_mutate_insert1();
	_mutate_insert1();
}

void NewRep::mutate(MUTATION_OPERATIONS mutation) {
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
	}
}

void NewRep::random_initialization() {
	std::vector<int> remaining(data.N_JOBS, data.N_MACHINES);
	std::vector<int> jobs(data.N_JOBS);
	for (int i = 0; i < data.N_JOBS; i++) {
		jobs[i] = i;
	}

	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		int index = rand() % jobs.size();
		int job = jobs[index];
		chromosome_string[i] = job;

		if (--remaining[job] == 0) {
			jobs.erase(jobs.begin() + index);
		}
	}
}

NewRep NewRep::operator =(const NewRep &rhs) {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		chromosome_string[i] = rhs.chromosome_string[i];
	}

	return *this;
}

std::ostream& operator << (std::ostream& out, const NewRep& chromosome) {
	out << chromosome.chromosome_string[0];

	for (int i = 1; i < data.N_JOBS*data.N_MACHINES; i++) {
		out << ", " << chromosome.chromosome_string[i];
	}

	return out;
}