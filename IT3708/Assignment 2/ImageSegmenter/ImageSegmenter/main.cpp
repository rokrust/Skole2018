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

	PhenotypeGenerator generator(&c1);
	std::vector<Segment> segment = generator.build_segments();
	generator.calculate_edge_value();
		
	system("PAUSE");
	return 0;
}
