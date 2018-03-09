#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>

void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Genotype genotype;
	MST mst;
	mst.genotype_generator(genotype);
	std::cout << genotype;


	system("PAUSE");
	return 0;
}
