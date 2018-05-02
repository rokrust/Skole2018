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


unsigned int Phenotype::_lowest_remaining_execution_time(std::vector<unsigned int> remaining_execution_time) {
	unsigned int lowest_value = 999999;
	
	for (int i = 0; i < data.N_MACHINES; i++) {
		if (lowest_value > remaining_execution_time[i] && remaining_execution_time[i] != 0) {
			lowest_value = remaining_execution_time[i];
		}
	}

	return lowest_value;
}

void Phenotype::_deadlock_handler() {
	//fitness = 1000;
}

void Phenotype::calculate_fitness() {
	std::vector<unsigned int> remaining_execution_time(data.N_MACHINES); //Time until each machine is done with its current job
	std::vector<unsigned int> progress(data.N_MACHINES); //How far along each job is

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++){
			std::cout << work_order[i][j] << '\t';
		}
		std::cout << std::endl;
	}

	fitness = 0;
	unsigned int jobs_left = data.N_JOBS;
	while (jobs_left) { //while jobs left
		bool no_job_scheduled = true;

		for (int machine = 0; machine < data.N_MACHINES; machine++) {
			
			if (remaining_execution_time[machine] == 0 && progress[machine] < data.N_JOBS - 1) {
				//Todo check if prerequisite jobs are done
				unsigned int next_job = work_order[machine][progress[machine]];
				progress[machine]++;
				remaining_execution_time[machine] = data.execution_time[next_job][machine];
				no_job_scheduled = false;

				std::cout << "\tMachine " << machine << ": Scheduled for job " << next_job << std::endl;

				if (progress[machine] == data.N_JOBS - 1) {
					jobs_left--; //One less job left
				}
			}
		}

		//Could not assign any jobs. Deadlock detected
		if (no_job_scheduled) {
			std::cout << "Deadlock detected!\n";
			_deadlock_handler();
			return;
		}
		

		unsigned int lowest_execution_time = _lowest_remaining_execution_time(remaining_execution_time);
		fitness += lowest_execution_time;
		for (int job = 0; job < data.N_JOBS; job++) {
			remaining_execution_time[job] -= lowest_execution_time;
		}
	}
}

/*
//Graph based approach
void Phenotype::calculate_fitness() {
	unsigned int adjacency_list** = new unsigned int

}
*/