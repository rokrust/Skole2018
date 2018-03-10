#include "image.hpp"
#include "Genetic_algorithm.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void print_index(Index index) {
	std::cout << '(' << index.row << ", " << index.col << ')';
}

int main() {
	Population population(8, 4, 0.2, 0.8); // 8s - 33s

	system("PAUSE");

	return 0;
}