#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Population population(20, 4, 0.2, 0.8);
	//system("PAUSE");
	while(1){
		std::cout << "Crossover\n";
		population.test();
		population.test2();
		std::cout << "Create phenotypes\n";
		//population.create_phenotypes();

		//Parent generation sorting
		std::cout << "Non dominated sort\n";
		//population.non_dominated_sort();

		std::cout << "Calculate crowding distance\n";
		//population.calculate_crowding_distances();
		//population.sort_pareto_fronts();
	}
	system("PAUSE");

	return 0;
}