#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>

void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	image.calculate_color_distances();

	system("PAUSE");
	return 0;
}
