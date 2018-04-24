//#include "data.h"
#include "genetics.h"

#include <cstdlib>

//Data data;

Chromosome::Chromosome() {

}

//okay
Chromosome::Chromosome(char* str): Chromosome() {
	for (int i = 0; str[i] != '\0'; i++) {
		chromosome_string[i] = str[i] - '0'; //translate to indices
	}
}

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

void OttoRep::convert_to_phenotype(unsigned int** phenotype) {
	//For each machine
	for (int comp = 0; comp < data.N_MACHINES; comp++) {
		unsigned int comp_base_i = comp*data.N_JOBS;
		
		//For each job of the machine
		for (int i = 0; i < data.N_JOBS; i++) {
			unsigned int job = chromosome_string[comp_base_i + i];

			//Iterate through all earlier jobs to translate 
			//relative index to absolute index
			for (int j = i - 1; j >= 0; j--) {
				if (chromosome_string[j] <= job) {
					job++;
				}
			}

			phenotype[comp][i] = job;
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