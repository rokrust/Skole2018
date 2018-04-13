#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>
#include <limits>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;
using namespace std::chrono;

/********************************** Population **********************************/
Population::Population(int population_size, int tournament_size,
	double mutation_rate, double crossover_rate, int max_segments) :
	population_size(population_size), tournament_size(tournament_size), 
	mutation_rate(mutation_rate), crossover_rate(crossover_rate), max_segments(max_segments) {
	population.reserve(population_size);

	std::vector<std::thread> thread_vec;
	for (int i = 0; i < population_size; i++) {
		population.push_back(Genotype());
		cout << "Thread " << i << " created, boii\n";
		Index root_node = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
		thread_vec.push_back(std::thread(&MST::genotype_generator, MST(), std::ref(population[i]), root_node));
	}

	for (int i = 0; i < population_size; i++) {
		std::cout << "Thread " << i << " joined\n";
		thread_vec[i].join();
	}

	/*
	int remaining_individuals = population_size;
	while (remaining_individuals) {
		for (int i = 0; i < n_concurrent_threads; i++) {
			std::cout << "Remaining " << remaining_individuals << std::endl;
			if (thread_vec[i].joinable()) {
				std::cout << "Joining thread " << i << std::endl;
				thread_vec[i].join();
				remaining_individuals--;

				//Replace the old thread while individuals still need to be initialized
				if (remaining_individuals >= n_concurrent_threads) {
					std::cout << "Creating thread\n";
					population.push_back(Genotype());
					Index root_node = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
					thread_vec[i] = std::thread(&MST::genotype_generator, MST(), std::ref(population.back()), root_node);
				}

			}
		}
		using namespace std::chrono_literals;
		using namespace std::chrono;
		std::this_thread::sleep_for(milliseconds(5));
	}
	
	for (int i = 0; i < population.size(); i++) {
		population[i].add_random_root_nodes();
	}
	*/
	cout << "DONNEEE!\n";

}

Population::Population(const Population& original) {
	this->tournament_size = original.tournament_size;
	this->population_size = original.population_size;
	this->mutation_rate = original.mutation_rate;
	this->crossover_rate = original.crossover_rate;
	this->population = original.population;
	this->max_segments = original.max_segments;
	this->population_phenotypes = original.population_phenotypes;
	this->pareto_fronts = original.pareto_fronts;

}

Population& Population::operator=(const Population& original) {
	this->tournament_size = original.tournament_size;
	this->population_size = original.population_size;
	this->mutation_rate = original.mutation_rate;
	this->crossover_rate = original.crossover_rate;
	this->population = original.population;
	this->population_phenotypes = original.population_phenotypes;
	this->pareto_fronts = original.pareto_fronts;
	this->max_segments = original.max_segments;

	return *this;
}

void Population::sort_pareto_fronts() {
	for (int current_front = 0; current_front < pareto_fronts.size(); current_front++) {
		sort_front_by_crowding_distance(current_front);
	}

}

struct Pair {
	double fitness;
	int index;

	bool operator<(const Pair& rhs) const { return fitness < rhs.fitness; }
};

void Population::weighted_sum_ga(double edge_weight, double deviation_weight) {
	for (int i = 0; i < population.size(); i++) {
		population_phenotypes.push_back(Phenotype(population[i]));
		population_phenotypes[i].index = i;
		population_phenotypes[i].calculate_edge_value();
		population_phenotypes[i].calculate_overall_deviation();

	}

	std::vector<Pair> storage;
	for (int i = 0; i < population_phenotypes.size(); i++) {
		double fitness = edge_weight*population_phenotypes[i].get_edge_value() + deviation_weight*population_phenotypes[i].get_overall_deviation();
		int index = i;
		Pair p = { fitness, i };
		storage.push_back(p);
	}

	std::sort(storage.begin(), storage.end());
	cout << storage[0].fitness;
}

void Population::calculate_crowding_distances() {
	for (int current_front  = 0; current_front < pareto_fronts.size(); current_front++) { //35
		std::vector<int> sorted_by_edge_value;
		std::vector<int> sorted_by_overall_deviation;

		sort_front_by_edge_value(current_front, sorted_by_edge_value);
		sort_front_by_overall_deviation(current_front, sorted_by_overall_deviation);

		//Calculate crowding distance for each individual
		calculate_crowding_distance_edge_contribution(sorted_by_edge_value);
		calculate_crowding_distance_deviation_contribution(sorted_by_overall_deviation);
	}
}

void Population::calculate_crowding_distance_edge_contribution(const std::vector<int>& sorted_front) {
	double edge_value_range = population_phenotypes[sorted_front.back()].get_edge_value() -
		population_phenotypes[sorted_front.front()].get_edge_value();

	//Calculate crowding distance for each individual
	for (int i = 1; i < sorted_front.size() - 1; i++) {

		//Edge calculation
		int left = sorted_front[i - 1];
		int right = sorted_front[i + 1];
		int current = sorted_front[i];
		double edge_value = population_phenotypes[current].get_crowding_distance();
		edge_value += (population_phenotypes[right].get_edge_value() -
			population_phenotypes[left].get_edge_value()) /
			edge_value_range;

		population_phenotypes[current].set_crowding_distance(edge_value);
	}
	
	double INF = std::numeric_limits<double>::infinity();
	population_phenotypes[sorted_front.front()].set_crowding_distance(INF);
	population_phenotypes[sorted_front.back()].set_crowding_distance(INF);
}

void Population::calculate_crowding_distance_deviation_contribution(const std::vector<int>& sorted_front) {
	double overall_deviation_range = population_phenotypes[sorted_front.back()].get_overall_deviation() -
		population_phenotypes[sorted_front.front()].get_overall_deviation();

	//Deviation calculation
	for (int i = 1; i < sorted_front.size() - 1; i++) {
		int left = sorted_front[i - 1];
		int right = sorted_front[i + 1];
		int current = sorted_front[i];
		double overall_deviation = population_phenotypes[current].get_crowding_distance();
		overall_deviation += (population_phenotypes[right].get_overall_deviation() -
			population_phenotypes[left].get_overall_deviation()) /
			overall_deviation_range;

		population_phenotypes[current].set_crowding_distance(overall_deviation);
	}

	double INF = std::numeric_limits<double>::infinity();
	population_phenotypes[sorted_front.front()].set_crowding_distance(INF);
	population_phenotypes[sorted_front.back()].set_crowding_distance(INF);
}

void Population::sort_front_by_crowding_distance(int front) {
	std::vector<int> sorted_front;
	std::vector<int> current_front = pareto_fronts[front];
	sorted_front.push_back(pareto_fronts[front][0]);

	for (int i = 0; i < current_front.size() - 1; i++) {
		
		int current_unsorted = current_front[i + 1];
		int current_sorted = sorted_front[i];

		if (population_phenotypes[current_sorted].get_crowding_distance() > population_phenotypes[current_unsorted].get_crowding_distance()) {

			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (population_phenotypes[sorted_front[j]].get_crowding_distance() <= population_phenotypes[current_unsorted].get_crowding_distance()) {
					sorted_front.insert(sorted_front.begin() + j + 1, current_unsorted);
					break;
				}
				else if (j == 0) {
					sorted_front.insert(sorted_front.begin(), current_unsorted);
				}
			}
		}
		else {
			sorted_front.push_back(current_unsorted);
		}
	}

	pareto_fronts[front] = sorted_front;
}

void Population::sort_front_by_edge_value(int front, std::vector<int>& sorted) {
	std::vector<int> current_front = pareto_fronts[front];
	sorted.push_back(current_front[0]);

	for (int i = 0; i < current_front.size() - 1; i++) {
		int current_unsorted = current_front[i + 1];
		int current_sorted = sorted[i];

		if (population_phenotypes[current_sorted].get_edge_value() > population_phenotypes[current_unsorted].get_edge_value()) {

			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (population_phenotypes[sorted[j]].get_edge_value() <= population_phenotypes[current_unsorted].get_edge_value()) {
					sorted.insert(sorted.begin() + j + 1, current_unsorted);
					break;
				}
				else if (j == 0) {
					sorted.insert(sorted.begin(), current_unsorted);
				}
			}
		}
		else {
			sorted.push_back(current_unsorted);
		}
	}
}

void Population::sort_front_by_overall_deviation(int front, std::vector<int>& sorted) {
	std::vector<int> current_front = pareto_fronts[front];
	sorted.push_back(current_front[0]);

	for (int i = 0; i < current_front.size() - 1; i++) {
		int current_unsorted = current_front[i + 1];
		int current_sorted = sorted[i];

		if (population_phenotypes[current_sorted].get_overall_deviation() > population_phenotypes[current_unsorted].get_overall_deviation()) {

			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (population_phenotypes[sorted[j]].get_overall_deviation() <= population_phenotypes[current_unsorted].get_overall_deviation()) {
					sorted.insert(sorted.begin() + j + 1, current_unsorted);
					break;
				}
				else if (j == 0) {
					sorted.insert(sorted.begin(), current_unsorted);
				}
			}
		}
		else {
			sorted.push_back(current_unsorted);
		}
	}
}

int Population::tournament_selection() {
	int winner = rand() % population.size();
	
	for (int k = 0; k < tournament_size - 1; k++) {
		int competitor = rand() % population.size();
		if (competitor < winner) {
			winner = competitor;
		}
	}

	for (int i = 0; i < pareto_fronts.size(); i++) {
		if (winner >= pareto_fronts[i].size()) {
			winner -= pareto_fronts[i].size();
			continue;
		}
		else {
			return pareto_fronts[i][winner];
		}
	}
	/*int i = rand() % population.size();
	//Phenotype* winner = &population_phenotypes[i];

	for (int k = 0; k < tournament_size-1; k++) {
		int i = rand() % population.size();
		Phenotype* current_competitor = &population_phenotypes[i];

		//pseudocode
		if (current_competitor->dominates(*winner)) {
			winner = current_competitor;
		}
		else if (!winner->dominates(*current_competitor)) {
			
		}
		
	}

	return winner->get_genotype();*/
}

int Population::pareto_ranked_tournament_selection() {
	int winner_rank = rand() % pareto_fronts.size();
	int winner_index = rand() % pareto_fronts[winner_rank].size();

	for (int k = 0; k < tournament_size - 1; k++) {
		int rank = rand() % pareto_fronts.size();
		int index = rand() % pareto_fronts[rank].size();

		if (rank > winner_rank) {
			winner_rank = rank;
			winner_index = index;
		}
		else if (rank == winner_rank) {
			if (index > winner_index) {
				winner_index = index;
			}
		}
	}
	
	return pareto_fronts[winner_rank][winner_index];
}

void Population::population_creation(Phenotype& phenotype, int i) {
	population_phenotypes[i].build_segments(population[i]);
	population_phenotypes[i].index = i;
	population_phenotypes[i].calculate_edge_value();
	population_phenotypes[i].calculate_overall_deviation();
}

void Population::create_phenotypes() {
	population_phenotypes.clear();

	cout << "Creating POPULESHAN\n";
	std::vector<std::thread> thread_vec;
	for (int i = 0; i < population.size(); i++) {
		population_phenotypes.push_back(Phenotype(population[i]));
		population_phenotypes[i].index = i;
		
/*		if (population_phenotypes[i].n_segments() > max_segments && population_phenotypes.size() > 50 && population.size() > 50) {
			population.erase(population.begin() + i);
			population_phenotypes.erase(population_phenotypes.begin() + i);
			i--;
		}*/
	}
	cout << "Population size: " << population.size() << endl;

	for (int i = 0; i < population.size(); i++) {
		population_phenotypes[i].calculate_edge_value();
		thread_vec.push_back(std::thread(&Phenotype::calculate_overall_deviation, std::ref(population_phenotypes[i])));
	}

	for (int i = 0; i < population.size(); i++) {
		//cout << "Thread " << i << " joined. CONGRATULEeASHAN\n";
		thread_vec[i].join();
	}
}

void Population::next_generation() {
	create_phenotypes();

	//Parent generation sorting
	non_dominated_sort();
	calculate_crowding_distances();
	sort_pareto_fronts();

	//Selection
	create_children();

	//Parent/child generation sorting
	create_phenotypes();
	non_dominated_sort();
	calculate_crowding_distances();
	sort_pareto_fronts();

	std::vector<Genotype> child_generation;
	int front_count = 0;
	
	std::cout << population_phenotypes[pareto_fronts[0].back()].get_edge_value() << ", " << population_phenotypes[pareto_fronts[0].back()].get_overall_deviation();
	std::cout << std::endl;

	while (child_generation.size() < population_size) {
		child_generation.push_back(population[pareto_fronts[front_count].back()]);
		pareto_fronts[front_count].pop_back();
		
		if (pareto_fronts[front_count].size() == 0) {
			front_count++;
		}
	}

	population = child_generation;
}

void Population::create_children() {
	for (int i = 0; i < population_size; i++) {
		int p1_index = pareto_ranked_tournament_selection();
		int p2_index = pareto_ranked_tournament_selection();

		//Genetic operators
		double crossover = (rand() % 100) / 100.0;
		double mutation = (rand() % 100) / 100.0;

		if (crossover <= crossover_rate) {
			Genotype c1, c2;
			population[p1_index].crossover_two_point(population[p2_index], c1, c2);
			population.push_back(c1);
			population.push_back(c2);
			continue;
		}
		else if (mutation <= mutation_rate) {
			population[p1_index].mutate();
			population[p2_index].mutate();
		}

		population.push_back(population[p1_index]);
		population.push_back(population[p1_index]);
	}

}

void Population::non_dominated_sort() {
	pareto_fronts.clear();

	std::vector<std::vector<int>> dominates(population.size());
	int* dominated_by = new int[population.size()] ();

	std::vector<int> next_front;

	//Find out who dominates whom
	for (int i = 0; i < population.size(); i++) {
		Phenotype* current_phenotype = &population_phenotypes[i];

		for (int j = i + 1; j < population.size(); j++) {
			
			if (current_phenotype->dominates(population_phenotypes[j])) {
				dominates[i].push_back(j);
				dominated_by[j]++;
			}

			else if (population_phenotypes[j].dominates(*current_phenotype)){
				dominates[j].push_back(i);
				dominated_by[i]++;
			}
		}
		
		if (dominated_by[i] == 0) {
			next_front.push_back(i);
		}
	}

	pareto_fronts.push_back(next_front);

	//Push next front to pareto fronts
	for (int front_counter = 0; next_front.size() != 0; front_counter++) {
		next_front.clear();

		//Iterate through the individuals at the front
		for (int i = 0; i < pareto_fronts[front_counter].size(); i++) {
			std::vector<int> current_front = pareto_fronts[front_counter];
			int current_dominating = current_front[i];


			for (int j = 0; j < dominates[current_dominating].size(); j++) {
				int current_dominated = dominates[current_dominating][j];
				dominated_by[current_dominated]--;

				if (dominated_by[current_dominated] == 0) {
					next_front.push_back(current_dominated);
				}
			}
		}

		pareto_fronts.push_back(next_front);
	}
	pareto_fronts.pop_back();
	delete dominated_by;
}

void Population::output_solution() {
	create_phenotypes();
	non_dominated_sort();
	calculate_crowding_distances();
	sort_pareto_fronts();

	Image contour_image(image);
	Image solution_image(image);
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			contour_image[row][col] = { 255, 255, 255 };
		}
	}
	population_phenotypes[pareto_fronts[0].back()].create_solution_image(contour_image, { 0, 0, 0 });
	population_phenotypes[pareto_fronts[0].back()].create_solution_image(solution_image, { 0, 255, 0 });

	/*char* image_dir[5] = { "contour0.jpg", "contour1.jpg" , "contour2.jpg" ,"contour3.jpg" ,"contour4.jpg" };
	for (int i = 0; i < pareto_fronts[0].size(); i++) {
		population_phenotypes[pareto_fronts[0][i]].create_solution_image(contour_image, { 0, 0, 0 });
		population_phenotypes[pareto_fronts[0][i]].create_solution_image(solution_image, { 0, 255, 0 });
		population_phenotypes[pareto_fronts[0].back()].create_solution_image(contour_image, { 0, 0, 0 });
		population_phenotypes[pareto_fronts[0].back()].create_solution_image(solution_image, { 0, 255, 0 });
		contour_image.write(image_dir[i]);
	}*/
	
	contour_image.write("contour.jpg");
	solution_image.write("solution.jpg");
}

void Population::join_segments(int max) {
	create_phenotypes();
	non_dominated_sort();
	calculate_crowding_distances();
	sort_pareto_fronts();

	population_phenotypes[pareto_fronts[0].back()].join_segments(max);
}

/********************************************************************************/

/*********************************** Phenotype **********************************/
Phenotype::Phenotype(const Phenotype& phenotype)
	: Phenotype(){
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->belongs_to_segment[row][col] = phenotype.belongs_to_segment[row][col];
			this->is_part_of_segment[row][col] = phenotype.is_part_of_segment[row][col];
		}
	}

	this->active_edge = phenotype.active_edge;
	this->new_active_edge = phenotype.active_edge;
	this->overall_deviation = phenotype.overall_deviation;
	this->edge_value = phenotype.edge_value;
	this->crowding_distance = phenotype.crowding_distance;
	this->segment = phenotype.segment;
	this->index = phenotype.index;

}

Phenotype& Phenotype::operator=(const Phenotype& rhs){
	
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->belongs_to_segment[row][col] = rhs.belongs_to_segment[row][col];
			this->is_part_of_segment[row][col] = rhs.is_part_of_segment[row][col];
		}
	}

	this->active_edge = rhs.active_edge;
	this->new_active_edge = rhs.active_edge;
	this->overall_deviation = rhs.overall_deviation;
	this->edge_value = rhs.edge_value;
	this->crowding_distance = rhs.crowding_distance;
	this->segment = rhs.segment;
	this->index = rhs.index;

	return *this;
}

bool Phenotype::dominates(const Phenotype& p2) {
	return (this->edge_value <= p2.edge_value && this->overall_deviation < p2.overall_deviation)
		|| (this->edge_value < p2.edge_value && this->overall_deviation <= p2.overall_deviation);
}

void Phenotype::add_ingoing_to_active_edge(int row, int col, const Genotype& genotype) {
	Index dummy = { -1, -1 };
	std::array<Index, 4> neighbor = { dummy, dummy, dummy, dummy };

	image.get_neighbors(row, col, neighbor);

	for (int pixel = 0; pixel < 4; pixel++) {
		Index current_neighbor = neighbor[pixel];
		if (current_neighbor.is_none()) {
			continue;
		}
		
		GRAPH_EDGE_DIR dir = genotype[current_neighbor.row][current_neighbor.col];
		Index next_neighbor = image.next_index(current_neighbor.row, current_neighbor.col, dir);
		if (next_neighbor.is_none()) {
			continue;
		}

		//Add neighbor if it points to the segment
		if (!is_part_of_segment[current_neighbor.row][current_neighbor.col] &&
			is_part_of_segment[next_neighbor.row][next_neighbor.col]) {
			
			is_part_of_segment[current_neighbor.row][current_neighbor.col] = true;
			new_active_edge.push_back(current_neighbor);
		}
	}

}

void Phenotype::add_outgoing_to_active_edge(int row, int col, GRAPH_EDGE_DIR dir) {
	if (dir != NONE) {
		Index next_pixel = image.next_index(row, col, dir);
		if (next_pixel.is_none()) {
			return;
		}

		// Add to segment
		if (!is_part_of_segment[next_pixel.row][next_pixel.col]) {
			is_part_of_segment[next_pixel.row][next_pixel.col] = true;
			new_active_edge.push_back(next_pixel);
		}

	}
}

void Phenotype::build_segment_from_pixel(int row, int col, int segment_id, const Genotype& genotype) {
	active_edge.push_back({ row, col });
	is_part_of_segment[row][col] = true;
	Segment current_segment;
	
	// As long as a pixel was added to the segment
	while (active_edge.size() != 0) {
		new_active_edge.clear();
		
		for (int i = active_edge.size() - 1; i >= 0; i--) {
			Index current_pixel = active_edge[i];
			
			current_segment.add_pixel(current_pixel.row, current_pixel.col);
			active_edge.pop_back();
			belongs_to_segment[current_pixel.row][current_pixel.col] = segment_id;
			
			//Add all neighbors pointing towards the segment
			add_ingoing_to_active_edge(current_pixel.row, current_pixel.col, genotype);	//Kalles mange ganger på samme pixel?

			//Add all neighbors being pointed to by the segment
			GRAPH_EDGE_DIR dir = genotype[current_pixel.row][current_pixel.col];
			add_outgoing_to_active_edge(current_pixel.row, current_pixel.col, dir);

		}

		//All members of the old edge handled. Go on to new edge
		active_edge = new_active_edge;
	}

	segment.push_back(current_segment);
}

void Phenotype::build_segments(const Genotype& genotype) {
	std::vector<Segment> segments;
	int segment_id = 0;
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			//Found a pixel that is not a part of a segment
			if (!is_part_of_segment[row][col]) {
				segment_id++;
				build_segment_from_pixel(row, col, segment_id, genotype);
			}

		}
	}
}

Phenotype::Phenotype() {
	is_part_of_segment = new bool*[IMAGE_HEIGHT];
	belongs_to_segment = new int*[IMAGE_HEIGHT];
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		is_part_of_segment[row] = new bool[IMAGE_WIDTH];
		belongs_to_segment[row] = new int[IMAGE_WIDTH];

		for (int col = 0; col < IMAGE_WIDTH; col++) {
			is_part_of_segment[row][col] = false;
			belongs_to_segment[row][col] = -1;
		}
	}

	crowding_distance = 0.0;
	edge_value = 0.0;
	overall_deviation = 0.0;
}

Phenotype::~Phenotype() {

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] is_part_of_segment[row];
		delete[] belongs_to_segment[row];
	}

}

Phenotype::Phenotype(Genotype& genotype): Phenotype() {
	build_segments(genotype);
}

void Phenotype::calculate_overall_deviation() {
	overall_deviation = 0.0;

	//For each segment
	for (std::vector<Segment>::iterator current_segment = segment.begin();
		current_segment != segment.end();
		current_segment++) {

		double current_deviation = 0;
		Pixel current_centroid = current_segment->calculate_centroid();

		//For each pixel of segment
		for (std::vector<Index>::iterator current_index = current_segment->begin();
			current_index != current_segment->end();
			current_index++) {
			Pixel current_pixel = image[current_index->row][current_index->col];
			current_deviation += current_pixel.color_distance(current_centroid);
		}

		overall_deviation += current_deviation;
	}
}

void Phenotype::calculate_edge_value() {
	edge_value = 0.0;

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			Index current_pixel = { row, col };
			std::array<Index, 4> neighbor = {};
			
			image.get_neighbors(current_pixel.row, current_pixel.col, neighbor);
			int current_pixel_id = belongs_to_segment[current_pixel.row][current_pixel.col];
			int current_neighbor_id;

			for (int i = 0; i < 4; i++) {
				Index current_neighbor = neighbor[i];
				if (current_neighbor.is_none()) { continue; }
				
				current_neighbor_id = belongs_to_segment[current_neighbor.row][current_neighbor.col];

				if (current_neighbor_id != current_pixel_id) {
					edge_value -= image.distance_between(current_pixel, current_neighbor);
				}
			}
		}
	}

}

void Phenotype::print_segments() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			std::cout << belongs_to_segment[row][col] << '\t';
		}
		std::cout << std::endl;
	}
}

void Phenotype::create_solution_image(Image& solution_image, Pixel color) {
	std::cout << segment.size() << std::endl;
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			std::array<Index, 4> neighbor;
			image.get_neighbors(row, col, neighbor);

			//See if neighbors belong to a different segment
			int current_segment = belongs_to_segment[row][col];
			for (int i = 0; i < 4; i++) {
				Index current_neighbor = neighbor[i];

				int neighbor_segment = belongs_to_segment[current_neighbor.row][current_neighbor.col];

				if (current_neighbor.is_none()) { continue; }

				//Pixel is at the edge. Change the color of it
				if (current_segment != neighbor_segment) {
					solution_image[row][col] = color;
				}
			}
		
		}
	}
}

void Phenotype::join_segments(int max_segments) {
	int smallest_segment = 0;
	int segment_size = numeric_limits<double>::infinity();
	int segment_size_blargh = segment.size();
	while(segment_size_blargh > max_segments){
		for (int i = 0; i < segment.size(); i++) {
			if (segment[i].get_length() < segment_size) {
				smallest_segment = i;
				segment_size = segment[i].get_length();
			}
		}
		
		segment_size = 0;
		int segment_id = 0;

		//Find largest neighboring segment
		for (int i = 0; i < segment[smallest_segment].get_length(); i++) {
			Index current_index = segment[smallest_segment][i];
			std::array<Index, 4> neighbor;
			image.get_neighbors(current_index.row, current_index.col, neighbor);
			int current_segment = belongs_to_segment[current_index.row][current_index.col];

			for (int j = 0; j < 4; j++) {
				int neighbor_segment = belongs_to_segment[neighbor[j].row][neighbor[j].col];
				if (current_segment != neighbor_segment && 
					segment[neighbor_segment - 1].get_length() > segment_size) {

					segment_size = segment[neighbor_segment - 1].get_length();
					segment_id = neighbor_segment;
				}
			}
		}

		//Join smallest segment to its largest neighbor
		for (int i = 0; i < segment[smallest_segment].get_length(); i++) {
			Index ind = segment[smallest_segment][i];
			belongs_to_segment[ind.row][ind.col] = segment_id;
			
		}
		segment_size_blargh--;

	}
}
/********************************************************************************/


/************************************* MST **************************************/

void MST::build_tree(Index start_index) {
	Node start_node = { 0, NONE, start_index };
	add_node_to_mst(start_node);
	add_neighbors_to_queue(start_node);

	while (true) {
		Node node = pop_from_queue();
		if (node.index.is_none()) { break; }

		add_node_to_mst(node);
		add_neighbors_to_queue(node);
	}
}

Node MST::pop_from_queue() {

	for (int i = priority_queue.size() - 1; i >= 0; i--) {
		Node current_node = priority_queue[i];
		
		priority_queue.pop_back();
		if (!in_tree[current_node.index.row][current_node.index.col]) {
			return current_node;
		}
	}

	return { 0, NONE, { -1, -1 } };
}

void MST::add_node_to_mst(Node node) {
	in_tree[node.index.row][node.index.col] = true;
	mst_set.push_back(node);
}

void MST::add_neighbors_to_queue(Node node) {
	std::array<Index, 4> neighbor;
	image.get_neighbors(node.index.row, node.index.col, neighbor);

	//Iterate through all neighbors of the added node
	for (int i = 0; i < 4; i++) {
		if (neighbor[i].is_none()) { continue; } //Skip invalid neighbors (out of bounds)

		//Add neighbors to the queue if not already in the tree
		if (!in_tree[neighbor[i].row][neighbor[i].col]) {
			double cost = image.distance_between(neighbor[i], node.index);
			GRAPH_EDGE_DIR dir = neighbor[i].get_direction_to(node.index);

			insert_in_queue({ cost, dir, neighbor[i] });

		}
	}
}

void MST::insert_in_queue(Node node) {
	for (int i = priority_queue.size() - 1; i >= 0; i--) {
		if (priority_queue[i].cost > node.cost) {
			priority_queue.insert(priority_queue.begin() + i, node);
			return;
		}
	}

	priority_queue.insert(priority_queue.begin(), node);
}

void MST::genotype_generator(Genotype& genotype, Index root_node) {
	build_tree(root_node);

	for (int i = 0; i < mst_set.size(); i++) {
		Node current_node = mst_set[i];
		genotype.edge[current_node.index.row][current_node.index.col] = current_node.dir;
	}

}

MST::MST() {
	const double INF = std::numeric_limits<double>::infinity();
	in_tree = new bool*[IMAGE_HEIGHT];

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		in_tree[row] = new bool[IMAGE_WIDTH];
	}


	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			in_tree[row][col] = false;

		}
	}
}

MST::MST(const MST &original): MST() {
	this->mst_set = original.mst_set;
}

MST& MST::operator=(const MST &rhs) {
	this->mst_set = rhs.mst_set;

	return *this;
}

MST::~MST() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] in_tree[row];
	}
	delete[] in_tree;
}

/********************************************************************************/



/*********************************** Genotype ***********************************/
Genotype::Genotype() {
	edge = new GRAPH_EDGE_DIR*[IMAGE_HEIGHT];
	
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		edge[row] = new GRAPH_EDGE_DIR[IMAGE_WIDTH];
	}
}

Genotype::Genotype(const Genotype& original): Genotype() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->edge[row][col] = original.edge[row][col];
		}
	}
}

Genotype::~Genotype() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		delete[] edge[row];
	}

	delete[] edge;
}

//Member functions
void Genotype::mutate() {
	unsigned int row = rand() % IMAGE_HEIGHT;
	unsigned int col = rand() % IMAGE_WIDTH;
	GRAPH_EDGE_DIR dir = static_cast<GRAPH_EDGE_DIR>(rand() % 5);

	edge[row][col] = dir;
}

void Genotype::crossover_one_point(const Genotype& p2, Genotype& c1, Genotype& c2) {
	const GRAPH_EDGE_DIR* p2_ptr = p2[0];
	GRAPH_EDGE_DIR* c1_ptr = c1[0];
	GRAPH_EDGE_DIR* c2_ptr = c2[0];
	
	int crossover_point = rand() % N_PIXELS;
	
	for(int i = 0; i < crossover_point; i++){
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
	}

	for (int i = crossover_point; i < N_PIXELS; i++) {
		c1_ptr[i] = p2_ptr[i];
		c2_ptr[i] = (*edge)[i];
	}
	
}

void Genotype::crossover_two_point(const Genotype& p2, Genotype& c1, Genotype& c2) {

	int crossover_point2_row = rand() % IMAGE_HEIGHT;
	int crossover_point2_col = rand() % IMAGE_WIDTH;
	
	int crossover_point1_row = rand() % IMAGE_HEIGHT;
	int crossover_point1_col = rand() % IMAGE_WIDTH;

	if (crossover_point2_row < crossover_point1_row) {
		int temp = crossover_point2_row;
		crossover_point2_row = crossover_point1_row;
		crossover_point1_row = temp;
	}

	for (int row = 0; row < crossover_point1_row; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = (*this)[row][col];
			c2[row][col] = p2[row][col];
		}
	}

	for (int row = crossover_point1_row + 1; row < crossover_point2_row; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			c2[row][col] = (*this)[row][col];
			c1[row][col] = p2[row][col];
		}
	}

	for (int row = crossover_point2_row + 1; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			c1[row][col] = (*this)[row][col];
			c2[row][col] = p2[row][col];
		}
	}

	//Row of the first crossover point
	for (int col = 0; col < crossover_point1_col; col++) {
		c1[crossover_point1_row][col] = (*this)[crossover_point1_row][col];
		c2[crossover_point1_row][col] = p2[crossover_point1_row][col];

	}
	for (int col = crossover_point1_col; col < IMAGE_WIDTH; col++) {
		c2[crossover_point1_row][col] = (*this)[crossover_point1_row][col];
		c1[crossover_point1_row][col] = p2[crossover_point1_row][col];
	}

	//Row of the second crossover point
	for (int col = 0; col < crossover_point2_col; col++) {
		c2[crossover_point2_row][col] = (*this)[crossover_point2_row][col];
		c1[crossover_point2_row][col] = p2[crossover_point2_row][col];

	}
	for (int col = crossover_point2_col; col < IMAGE_WIDTH; col++) {
		c1[crossover_point2_row][col] = (*this)[crossover_point2_row][col];
		c2[crossover_point2_row][col] = p2[crossover_point2_row][col];
	}
	
}

Genotype& Genotype::operator=(const Genotype &rhs) {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->edge[row][col] = rhs.edge[row][col];
		}
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, Genotype genotype) {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			switch (genotype.edge[row][col]) {
			case(LEFT):
				os << "<\t";
				break;
			case(RIGHT):
				os << ">\t";
				break;
			case(UP):
				os << "^\t";
				break;
			case(DOWN):
				os << "v\t";
				break;
			case(NONE):
				os << "o\t";
				break;
			}
		}
		os << std::endl;
	}
	return os;
}

void Genotype::add_random_root_nodes() {
	for (int i = 0; i < 280; i++) {
		Index new_root = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
		edge[new_root.row][new_root.col] = NONE;
	}
}

void Population::test() {
	std::cout << population_phenotypes[pareto_fronts[0].back()].get_edge_value() << ", " << population_phenotypes[pareto_fronts[0].back()].get_overall_deviation();
	std::cout << std::endl;
}
/********************************************************************************/ 