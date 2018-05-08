#include "data.h"
#include <iostream>
#include <fstream>
#include <string>

Data::Data(char* dir) {
	std::ifstream file(dir, std::ios::in);
	
	//Read first line: N_JOBS and N_MACHINES
	file >> N_JOBS >> N_MACHINES;
	
	//Allocate arrays
	_allocate_members();

	//Store every execution time and job order in the corresponding class members
	int exec, job;
	int machine_nbr = 0, job_nbr = 0;

	while (file >> job >> exec) {
		work_order[job_nbr][machine_nbr] = job;
		execution_time[job_nbr][machine_nbr] = exec;
		machine_nbr++;

		//New machine and job order after new line
		if (machine_nbr == N_MACHINES) {
			job_nbr++;
			machine_nbr = 0;
		}
	}
	std::cout << *this;

	file.close();
}

Data::Data(const Data& data): N_MACHINES(data.N_MACHINES), N_JOBS(data.N_JOBS) {

	//Allocate arrays
	_allocate_members();

	//Copy arrays
	for (int job = 0; job < N_JOBS; job++) {
		for (int machine = 0; machine < N_MACHINES; machine++) {
			execution_time[job][machine] = data.execution_time[job][machine];
			work_order[job][machine] = data.work_order[job][machine];
		}
	}
}

Data Data::operator =(const Data& data) {
	N_MACHINES = data.N_MACHINES; 
	N_JOBS = data.N_JOBS;

	//Allocate arrays
	_allocate_members();

	//Copy arrays
	for (int job = 0; job < N_JOBS; job++) {
		for (int machine = 0; machine < N_MACHINES; machine++) {
			execution_time[job][machine] = data.execution_time[job][machine];
			work_order[job][machine] = data.work_order[job][machine];
		}
	}

	return *this;
}

Data::~Data() {
	for (int i = 0; i < N_JOBS; i++) {
		delete[] work_order[i];
		delete[] execution_time[i];
	}

	delete[] work_order;
	delete[] execution_time;
}

void Data::_allocate_members() {
	execution_time = new int*[N_JOBS];
	work_order = new int*[N_JOBS];
	
	for (int i = 0; i < N_JOBS; i++) {
		execution_time[i] = new int[N_MACHINES];
		work_order[i] = new int[N_MACHINES];
	}
}

std::ostream& operator << (std::ostream& out, const Data& d) {
	for (int job = 0; job < d.N_JOBS; job++) {
		for (int machine = 0; machine < d.N_MACHINES; machine++) {
			out << d.work_order[job][machine] << '\t';
		}
		out << std::endl;
	}

	out << std::endl;

	for (int job = 0; job < d.N_JOBS; job++) {
		for (int machine = 0; machine < d.N_MACHINES; machine++) {
			out << d.execution_time[job][machine] << '\t';
		}
		out << std::endl;
	}

	return out;
}