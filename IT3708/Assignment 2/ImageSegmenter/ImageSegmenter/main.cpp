#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Image solution(image);
	Population population(50, 4, 0.2, 0.8);
	//system("PAUSE");
	int kjell_tore = 0;
	while(kjell_tore++ < 100){
		std::cout << kjell_tore << std::endl;
		population.next_generation();
	}
	
	population.output_solution();

	system("PAUSE");
	
	return 0;
}