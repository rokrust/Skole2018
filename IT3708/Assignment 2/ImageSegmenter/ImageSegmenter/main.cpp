#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Population population(10, 4, 4, 0.2, 0.8);
	//system("PAUSE");
	population.create_phenotypes();
	//system("PAUSE");
	population.non_dominated_sort();
	system("PAUSE");

	return 0;
}