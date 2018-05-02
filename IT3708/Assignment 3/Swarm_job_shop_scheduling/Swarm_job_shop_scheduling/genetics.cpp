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
//okay
OttoRep::OttoRep(char* str): OttoRep(){
	for (int i = 0; str[i] != '\0'; i++) {
		chromosome_string[i] = str[i] - '0'; //translate to indices
	}
}

//okay
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

void OttoRep::mutate() { 
	unsigned int i = rand() % INDIRECT_CHROMOSOME_LENGTH;
	unsigned int new_val = rand() % (INDIRECT_CHROMOSOME_LENGTH - i - 1);

	chromosome_string[i] = new_val;
}

void OttoRep::convert_to_phenotype(Phenotype& phenotype) {
	//For each machine
	for (int comp = 0; comp < data.N_MACHINES; comp++) {
		unsigned int comp_base_i = comp*data.N_JOBS;
		
		//For each job of the machine
		for (int i = 0; i < data.N_JOBS; i++) {
			unsigned int job = chromosome_string[comp_base_i + i];

			//Iterate through all earlier jobs to translate 
			//relative index to absolute index
			for (int j = i - 1; j >= 0; j--) {
				if (chromosome_string[comp_base_i + j] <= job) {
					job++;
				}
			}

			phenotype.add_job(comp, i, job);
		}
	}
}

bool OttoRep::operator == (const OttoRep &rhs) {
	for (int i = 0; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
		if (this->chromosome_string[i] != rhs.chromosome_string[i]) {
			return false;
		}
	}

	return true;
}

bool OttoRep::operator == (const OttoRep &rhs) const {
	for (int i = 0; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
		if (this->chromosome_string[i] != rhs.chromosome_string[i]) {
			return false;
		}
	}

	return true;
}

std::ostream& operator << (std::ostream& out, const OttoRep& chromosome) {
	out << chromosome.chromosome_string[0];

	for (int i = 1; i < INDIRECT_CHROMOSOME_LENGTH; i++) {
		out << ", " << chromosome.chromosome_string[i];
	}

	return out;
}

//Phenotype
Phenotype::Phenotype() {
	work_order = new unsigned int*[data.N_MACHINES];
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		work_order[machine] = new unsigned int[data.N_JOBS];
	}
}

Phenotype::~Phenotype() {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		delete[] work_order[machine];
	}

	delete[] work_order;
}

PhenotypeScheduler::PhenotypeScheduler() {
	remaining_execution_time.resize(data.N_MACHINES, 0);
	machine_progress.resize(data.N_MACHINES, 0);
	job_progress.resize(data.N_JOBS, 0);
	required_machine.resize(data.N_JOBS, 0);
	
	no_job_scheduled = true;
	jobs_left = data.N_JOBS;
	total_execution_time = 0;

	//Set the first machine required for each job.
	for (int job = 0; job < data.N_JOBS; job++) {
		unsigned int beginning_machine = data.work_order[job][0] - 1;
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

	return lowest_value;
}

void PhenotypeScheduler::deadlock_handler() {
	//fitness = 1000;
}

void PhenotypeScheduler::execution_step(unsigned int time_step, unsigned int** work_order, unsigned int fitness) {
	for (int machine = 0; machine < data.N_MACHINES; machine++) {
		remaining_execution_time[machine] -= time_step;

		//Machine finished executing the current job
		if (remaining_execution_time[machine] == 0 && machine_progress[machine] != data.N_JOBS) {
			unsigned int finished_job = work_order[machine][machine_progress[machine]];
			std::cout << fitness << ": Machine " << machine << ", Finished executing job " << finished_job << std::endl;

			machine_progress[machine]++;	//Update progress for the machine
			job_progress[finished_job]++;	//Update progress for the job
			
			required_machine[finished_job] = data.work_order[finished_job][job_progress[finished_job]] - 1; //jobs are one indexed

			//Every job worked on. Machine is finished.
			if (machine_progress[machine] == data.N_JOBS) {
				jobs_left--;
			}

			no_job_scheduled = false;
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
				std::cout << "Machine " << machine << ": Assigned to job " << next_job << std::endl;
				remaining_execution_time[machine] = data.execution_time[next_job][job_progress[next_job]];
				no_job_scheduled = false;
			}
		}

		//The machine has to wait until another job is finished.
		else {
			std::cout << "Machine " << machine << ": Waiting to run job " << next_job << std::endl;
			remaining_execution_time[machine] = -1;
		}

	}

}

void Phenotype::calculate_fitness() {
	fitness = 0;
	PhenotypeScheduler scheduler;
	scheduler.assign_jobs(work_order);
	while (scheduler.jobs_left) {
		scheduler.no_job_scheduled = true;

		unsigned int time_step = scheduler.lowest_remaining_execution_time();
		fitness += time_step;
		scheduler.execution_step(time_step, work_order, fitness);

		scheduler.assign_jobs(work_order);

		if (scheduler.no_job_scheduled) {
			std::cout << "Deadlock detected!\n";
			scheduler.deadlock_handler();
		}
	}

}

/*
//Graph based approach
void Phenotype::calculate_fitness() {
	unsigned int adjacency_list** = new unsigned int

}
*/