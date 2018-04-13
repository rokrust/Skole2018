#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Population population(50, 4, 0.2, 0.8, 50);
	
	int generation_count = 0;
	while(generation_count++ < 50){
		std::cout << generation_count << std::endl;
		population.next_generation();
	}

	population.output_solution();
	system("PAUSE");
	
	return 0;
}