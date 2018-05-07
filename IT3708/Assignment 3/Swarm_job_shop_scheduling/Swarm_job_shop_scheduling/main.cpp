#include "data.h"
#include "genetics.h"
#include "config.h"
#include "artificial_bee.h"

#include <iostream>


Data data;

int main(char* dir) {
	std::cout << "Reading data\n";
	data = Data("../Test Data/0.txt");

	NewRep new_gene("010112202");
	Phenotype new_pheno;
	new_gene.convert_to_phenotype(new_pheno);
	new_pheno.calculate_fitness();
	std::cout << new_pheno.get_fitness() << std::endl;
	//Hive hive;
	//int acceptable_value = OPTIMAL_VALUE / (1 - 0.1);
	//hive.run_optimization(acceptable_value);
	//hive.run_optimization();
	

	system("PAUSE");
	return 0;
}