#include "data.h"
#include "genetics.h"
#include "config.h"
#include "artificial_bee.h"

#include <iostream>


Data data;

int main(char* dir) {
	std::cout << "Reading data\n";
	data = Data("../Test Data/1.txt");

	Hive hive;
	int acceptable_value = OPTIMAL_VALUE / (1 - 0.1);
	hive.run_optimization(acceptable_value);
	

	system("PAUSE");
	return 0;
}