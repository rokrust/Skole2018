#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Population population(8, 4, 0.2, 0.8);
	/*
	MST mst1;
	Genotype genotype1;
	MST mst2;
	Genotype genotype2;
	std::thread t1(&MST::genotype_generator, mst1, genotype1);
	std::thread t2(&MST::genotype_generator, mst2, genotype2);
	t1.join();
	t2.join();
	*/
	
	system("PAUSE");

	return 0;
}
