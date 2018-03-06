#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>

void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Genotype p1, p2;
	Genotype c1, c2;
	p1.test();
	p2.test();

	p1.crossover_two_point(p2, c1, c2);
	std::cout << c1 << std::endl << std::endl;

	Phenotype generator(c1);
	double edge_value = generator.calculate_edge_value();
	double overall_deviation = generator.calculate_overall_deviation();
		
	system("PAUSE");
	return 0;
}
