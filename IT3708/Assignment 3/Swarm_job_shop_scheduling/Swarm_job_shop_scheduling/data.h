#pragma once
#include <iostream>
class Data {
private:
	void _allocate_members();

public:
	int N_JOBS;
	int N_MACHINES;

	int** execution_time;
	int** work_order;

	Data() { ; }
	Data(char* dir);
	Data(const Data& data);

	~Data();
	Data operator=(const Data& data);

	friend std::ostream& operator << (std::ostream& out, const Data& data);
};

extern Data data;