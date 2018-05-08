#include "data.h"
#include "genetics.h"
#include "config.h"
#include "artificial_bee.h"
#include "ant_colony.h"
#include "util.h"

#include <iostream>
#include <vector>
#include <string>

Data data;

int main(int argc, char* argv[]) {
	/*
	char *dir = argv[1];
	data = Data(dir);

	if (argc >= 3) {
		int optimal_value = 0;
		if (argc == 4) {
			std::cout << argv[3] << std::endl;
			optimal_value = atoi(argv[3]);
		}

		std::cout << argv[2] << std::endl;
		if (std::string(argv[2]) == "ACO") {
			std::cout << "Running ant colony\n";
			AntColony colony;
			colony.run_optimization(optimal_value);
		}
		else if (std::string(argv[2]) == "ABC") {
			std::cout << "Running bee colony\n";
			Hive hive;
			hive.run_optimization(optimal_value);
		}
	}
	*/
	
	data = Data(TEST_DIR);
	AntColony colony;
	colony.run_optimization();

	system("PAUSE");
	return 0;
}