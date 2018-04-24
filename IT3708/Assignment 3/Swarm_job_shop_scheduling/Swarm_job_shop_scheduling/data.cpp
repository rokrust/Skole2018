#include "data.h"
#include <iostream>
#include <fstream>
#include <string>

Data::Data(char* dir) {
	std::string line;
	std::ifstream file(dir, std::ios::in);

	if (file.is_open()) {
		std::cout << "File opened!\n";
	}
	
	//Read first line: N_JOBS and N_MACHINES
	file >> N_JOBS >> N_MACHINES;

	//Allocate arrays
	execution_time = new unsigned int*[N_MACHINES];
	work_order = new unsigned int*[N_MACHINES];
	for (int i = 0; i < N_MACHINES; i++) {
		execution_time[i] = new unsigned int[N_JOBS];
		work_order[i] = new unsigned int[N_JOBS];
	}

	//Store every execution time and job order in the corresponding class members
	unsigned int exec, job;
	unsigned int machine_nbr = 0, job_nbr = 0;

	while (file >> job >> exec) {

		work_order[machine_nbr][job_nbr] = job;
		execution_time[machine_nbr][job_nbr] = exec;
		job_nbr++;

		//New machine and job order after new line
		if (job_nbr == N_JOBS) {
			machine_nbr++;
			job_nbr = 0;
		}
	}

	file.close();
	std::cout << "File closed!\n";
}

Data::Data(const Data& data): N_MACHINES(data.N_MACHINES), N_JOBS(data.N_JOBS) {

	//Allocate arrays
	execution_time = new unsigned int*[N_MACHINES];
	work_order = new unsigned int*[N_MACHINES];
	for (int i = 0; i < N_MACHINES; i++) {
		execution_time[i] = new unsigned int[N_JOBS];
		work_order[i] = new unsigned int[N_JOBS];
	}

	//Copy arrays
	for (int machine = 0; machine < N_MACHINES; machine++) {
		for (int job = 0; job < N_JOBS; job++) {
			execution_time[machine][job] = data.execution_time[machine][job];
			work_order[machine][job] = data.work_order[machine][job];
		}
	}
}

Data Data::operator =(const Data& data) {
	N_MACHINES = data.N_MACHINES; 
	N_JOBS = data.N_JOBS;

	//Allocate arrays
	execution_time = new unsigned int*[N_MACHINES];
	work_order = new unsigned int*[N_MACHINES];
	for (int i = 0; i < N_MACHINES; i++) {
		execution_time[i] = new unsigned int[N_JOBS];
		work_order[i] = new unsigned int[N_JOBS];
	}

	//Copy arrays
	for (int machine = 0; machine < N_MACHINES; machine++) {
		for (int job = 0; job < N_JOBS; job++) {
			execution_time[machine][job] = data.execution_time[machine][job];
			work_order[machine][job] = data.work_order[machine][job];
		}
	}

	return *this;
}

Data::~Data() {
	for (int i = 0; i < N_MACHINES; i++) {
		delete[] work_order[i];
		delete[] execution_time[i];
	}

	delete[] work_order;
	delete[] execution_time;
}