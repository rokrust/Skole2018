#include "data.h"
#include "genetics.h"
#include "config.h"
#include <iostream>

Data data;

int main(char* dir) {
	std::cout << "Reading data\n";
	data = Data("../Test Data/0.txt");

	char* str_best = "000200110";

	char* str_worst = "000000100";
	
	std::cout << "Genotypes created\n";
	OttoRep best_gene(str_best), worst_gene(str_worst);

	std::cout << "Phenotypes created\n";
	Phenotype best, worst;
	best_gene.convert_to_phenotype(best);
	worst_gene.convert_to_phenotype(worst);

	std::cout << "Calculating fitness\n";
	best.calculate_fitness();
	std::cout << std::endl << std::endl;
	worst.calculate_fitness();

	std::cout << "Best gene: " << best.get_fitness() << std::endl;
	std::cout << "Worst gene: " << worst.get_fitness() << std::endl;
	
	system("PAUSE");
	return 0;
}