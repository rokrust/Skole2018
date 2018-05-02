#pragma once
class Data {
private:
	void _allocate_members();

public:
	unsigned int N_JOBS;
	unsigned int N_MACHINES;

	unsigned int** execution_time;
	unsigned int** work_order;

	Data() { ; }
	Data(char* dir);
	Data(const Data& data);

	~Data();
	Data operator=(const Data& data);
};

extern Data data;