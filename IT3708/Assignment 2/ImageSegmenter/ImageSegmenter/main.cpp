#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>

int main() {
	Genotype p1, p2;
	Genotype c1, c2;
	p1.test();
	p2.test();

	std::cout << p1;
	std::cout << "|\n";
	std::cout << p2;
	std::cout << std::endl << "---------------------------------\n\n";

	p1.crossover_two_point(p2, c1, c2);
	std::cout << c1;
	std::cout << "|\n";
	std::cout << c2;
	
	system("PAUSE");
	return 0;
}