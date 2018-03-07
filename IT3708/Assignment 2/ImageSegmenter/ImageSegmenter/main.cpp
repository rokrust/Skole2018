#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>

void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Genotype genotype[50];
	int j = 0;
	system("PAUSE");
	for (int i = 0; i < 50; i++) {
		genotype[i].random_init();
		Phenotype dummy(genotype[i]);
	}
	system("PAUSE");
	//MST mst;
	//mst.genotype_generator(genotype);
	
	//std::cout << genotype;

	system("PAUSE");
	return 0;
}
