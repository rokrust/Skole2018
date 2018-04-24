#include "data.h"
#include "genetics.h"
#include "config.h"
#include <iostream>

//Data data;

int main(char* dir) {
	data = Data("../Test Data/1.txt");
	std::cout << data.N_JOBS << std::endl;
	std::cout << data.work_order[3][4] << std::endl;
	system("PAUSE");
	return 0;
}