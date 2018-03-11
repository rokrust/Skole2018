#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>
#include <limits>
#include <array>
#include <chrono>
#include <thread>
/********************************** Population **********************************/
Population::Population(int population_size, int tournament_size, 
					   double mutation_rate, double crossover_rate, 
					   int n_concurrent_threads) :
	tournament_size(tournament_size), mutation_rate(mutation_rate), crossover_rate(crossover_rate) {
	population.reserve(population_size);

	std::vector<std::thread> thread_vec;
	for (int i = 0; i < n_concurrent_threads; i++) {
		population.push_back(Genotype());
		Index root_node = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
		thread_vec.push_back(std::thread(&MST::genotype_generator, MST(), std::ref(population[i]), root_node));
	}

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
}

void Population::sort_pareto_fronts() {
	for (std::vector<std::vector<Phenotype*>>::iterator current_front = pareto_fronts.begin();
		current_front != pareto_fronts.end(); current_front++) {
		sort_front_by_crowding_distance(*current_front);
	}
}

void Population::calculate_crowding_distances() {
	for (std::vector<std::vector<Phenotype*>>::iterator current_front = pareto_fronts.begin();
		current_front != pareto_fronts.end(); current_front++) {

		std::vector<int> sorted_by_edge_value;
		std::vector<int> sorted_by_overall_deviation;

		sort_front_by_edge_value(*current_front, sorted_by_edge_value);
		sort_front_by_overall_deviation(*current_front, sorted_by_overall_deviation);

		//Difference between individuals at the edges
		double edge_value_range = (*current_front)[sorted_by_edge_value.back()]->get_edge_value() -
			(*current_front)[sorted_by_edge_value.front()]->get_edge_value();
		double overall_deviation_range = (*current_front)[sorted_by_overall_deviation.back()]->get_overall_deviation() -
			(*current_front)[sorted_by_overall_deviation.front()]->get_overall_deviation();

		//Calculate crowding distance for each individual
		calculate_crowding_distance_edge_contribution(*current_front, sorted_by_edge_value);
		calculate_crowding_distance_deviation_contribution(*current_front, sorted_by_overall_deviation);
	}
}

void Population::calculate_crowding_distance_edge_contribution(const std::vector<Phenotype*> front, const std::vector<int>& sorted_by_object_function) {
	double edge_value_range = front[sorted_by_object_function.back()]->get_edge_value() -
		front[sorted_by_object_function.front()]->get_edge_value();

	//Calculate crowding distance for each individual
	for (int i = 1; i < front.size() - 1; i++) {

		//Edge calculation
		int current_individual = sorted_by_object_function[i];
		int left_individual = sorted_by_object_function[i - 1];
		int right_individual = sorted_by_object_function[i + 1];
		Phenotype* current_individual_ptr = front[current_individual];

		double edge_value = current_individual_ptr->get_crowding_distance();
		edge_value += (front[right_individual]->get_edge_value() -
			front[left_individual]->get_edge_value()) /
			edge_value_range;

		current_individual_ptr->set_crowding_distance(edge_value);
	}
	
	double INF = std::numeric_limits<double>::infinity();
	front[sorted_by_object_function.front()]->set_crowding_distance(INF);
	front[sorted_by_object_function.back()]->set_crowding_distance(INF);
}

void Population::calculate_crowding_distance_deviation_contribution(const std::vector<Phenotype*> front, const std::vector<int>& sorted_by_object_function) {
	double overall_deviation_range = front[sorted_by_object_function.back()]->get_overall_deviation() -
		front[sorted_by_object_function.front()]->get_overall_deviation();

	//Calculate crowding distance for each individual
	for (int i = 1; i < front.size() - 1; i++) {

		//Edge calculation
		int current_individual = sorted_by_object_function[i];
		int left_individual = sorted_by_object_function[i - 1];
		int right_individual = sorted_by_object_function[i + 1];
		Phenotype* current_individual_ptr = front[current_individual];

		double overall_deviation = current_individual_ptr->get_crowding_distance();
		overall_deviation += (front[right_individual]->get_overall_deviation() -
			front[left_individual]->get_overall_deviation()) /
			overall_deviation_range;

		current_individual_ptr->set_crowding_distance(overall_deviation);
	}

	double INF = std::numeric_limits<double>::infinity();
	front[sorted_by_object_function.front()]->set_crowding_distance(INF);
	front[sorted_by_object_function.back()]->set_crowding_distance(INF);
}

void Population::sort_front_by_crowding_distance(std::vector<Phenotype*>& front) {
	std::vector<Phenotype*> sorted_front;
	sorted_front.push_back(front[0]);

	for (int i = 0; i < front.size() - 1; i++) {
		
		std::cout << front[i]->get_crowding_distance() << ", " << front[i + 1]->get_crowding_distance() << std::endl;
		Phenotype* insert = front[i + 1];
		if (sorted_front[i]->get_crowding_distance() > insert->get_crowding_distance()) {

			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (sorted_front[j]->get_crowding_distance() <= insert->get_crowding_distance()) {
					sorted_front.insert(sorted_front.begin() + j + 1, insert);
					break;
				}
				else if (j == 0) {
					sorted_front.insert(sorted_front.begin(), insert);
				}
			}
		}
		else {
			sorted_front.push_back(insert);
		}
	}

	front = sorted_front;
}

void Population::sort_front_by_edge_value(const std::vector<Phenotype*>& front, std::vector<int>& sorted) {
	sorted.push_back(0);
	for (int i = 0; i < front.size()-1; i++) {
		
		if (front[sorted[i]]->get_edge_value() > front[i + 1]->get_edge_value()) {
			
			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (front[sorted[j]]->get_edge_value() <= front[i + 1]->get_edge_value()) {
					sorted.insert(sorted.begin() + j + 1, i + 1);
					break;
				}
				else if(j == 0) {
					sorted.insert(sorted.begin(), i + 1);
				}
			}
		}
		else {
			sorted.push_back(i + 1);
		}
	}

}

void Population::sort_front_by_overall_deviation(const std::vector<Phenotype*>& front, std::vector<int>& sorted) {
	sorted.push_back(0);
	for (int i = 0; i < front.size() - 1; i++) {
		
		if (front[sorted[i]]->get_overall_deviation() > front[i + 1]->get_overall_deviation()) {

			//Iterate backwards to find the insertion point
			for (int j = i; j >= 0; j--) {
				if (front[sorted[j]]->get_overall_deviation() <= front[i + 1]->get_overall_deviation()) {
					sorted.insert(sorted.begin() + j + 1, i + 1);
					break;
				}
				else if (j == 0) {
					sorted.insert(sorted.begin(), i + 1);
				}
			}
		}
		else {
			sorted.push_back(i + 1);
		}
	}
}

Phenotype* Population::tournament_selection() {
	int i = rand() % population.size();
	Phenotype* winner = &population_phenotypes[i];

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

	return winner;
}

Phenotype* Population::pareto_ranked_tournament_selection() {
	int winner_rank = rand() % pareto_fronts.size();
	int winner_index = rand() % pareto_fronts[winner_rank].size();

	for (int k = 0; k < tournament_size - 1; k++) {
		int rank = rand() % pareto_fronts.size();
		int index = rand() % pareto_fronts[rank].size();

		if (rank > winner_rank) {
			winner_rank = rank;
		}
		else if (rank == winner_rank) {
			if (index > winner_index) {
				winner_index = index;
			}
		}
	}

	return pareto_fronts[winner_rank][winner_index];
}

void Population::create_phenotypes() {
	for (int i = 0; i < population.size(); i++) {
		population_phenotypes.push_back(Phenotype(population[i]));
		population_phenotypes[i].calculate_edge_value();
		population_phenotypes[i].calculate_overall_deviation();
	}
}

void Population::next_generation() {
	create_phenotypes();

	//Solution sorting
	non_dominated_sort();
	calculate_crowding_distances();
	sort_pareto_fronts();

	//Selection

	//Genetic operators
	double crossover = (rand() % 100) / 100.0;
	double mutation = (rand() % 100) / 100.0;
	if (crossover <= crossover_rate) {
	
	}
	if (mutation <= mutation_rate) {
	
	}

}

void Population::non_dominated_sort() {
	std::vector<std::vector<int>> fronts;
	std::vector<std::vector<int>> dominates(population.size());
	int* dominated_by = new int[population.size()] ();

	std::vector<Phenotype*> next_front_phenotype;
	std::vector<int> next_front_index;

	//Find out who dominates whom
	for (int i = 0; i < population.size(); i++) {
		Phenotype* current_phenotype = &population_phenotypes[i];

		for (int j = i+1; j < population.size(); j++) {
			
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
			next_front_phenotype.push_back(&population_phenotypes[i]);
			next_front_index.push_back(i);
		}
	}

	pareto_fronts.push_back(next_front_phenotype);
	fronts.push_back(next_front_index);

	//Push phenotype pointers to the pareto fronts
	for (int front_counter = 0; next_front_index.size() != 0; front_counter++) {
		next_front_index.clear();
		next_front_phenotype.clear();

		for (int i = 0; i < fronts[front_counter].size(); i++) {
			int current_dominating = fronts[front_counter][i];

			for (int j = 0; j < dominates[current_dominating].size(); j++) {
				int current_dominated = dominates[current_dominating][j];
				dominated_by[current_dominated]--;

				if (dominated_by[current_dominated] == 0) {
					next_front_phenotype.push_back(&population_phenotypes[current_dominated]);
					next_front_index.push_back(current_dominated);
					std::cout << current_dominated << '\t';
				}
			}
		}

		fronts.push_back(next_front_index);
		pareto_fronts.push_back(next_front_phenotype);
		std::cout << std::endl;
	}
	pareto_fronts.pop_back();
	delete dominated_by;
}

/********************************************************************************/

/*********************************** Phenotype **********************************/
Phenotype::Phenotype(const Phenotype& phenotype)
	: Phenotype(){
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->is_part_of_segment[row][col] = phenotype.is_part_of_segment[row][col];
			this->belongs_to_segment[row][col] = phenotype.belongs_to_segment[row][col];
		}
	}
	this->overall_deviation = phenotype.overall_deviation;
	this->edge_value = phenotype.edge_value;
	this->crowding_distance = phenotype.crowding_distance;
	this->segment = phenotype.segment;
}

Phenotype& Phenotype::operator=(const Phenotype& rhs) {
	
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->is_part_of_segment[row][col] = rhs.is_part_of_segment[row][col];
			this->belongs_to_segment[row][col] = rhs.belongs_to_segment[row][col];
		}
	}

	this->overall_deviation = rhs.overall_deviation;
	this->edge_value = rhs.edge_value;
	this->crowding_distance = rhs.crowding_distance;
	this->segment = rhs.segment;

	return *this;
}

bool Phenotype::dominates(const Phenotype& p2) {
	return (this->edge_value <= p2.edge_value && this->overall_deviation < p2.overall_deviation)
		|| (this->edge_value < p2.edge_value && this->overall_deviation <= p2.overall_deviation);
}

void Phenotype::add_ingoing_to_active_edge(int row, int col) {
	Index dummy = { -1, -1 };
	std::array<Index, 4> neighbor = { dummy, dummy, dummy, dummy };

	image.get_neighbors(row, col, neighbor);

	for (int pixel = 0; pixel < 4; pixel++) {
		Index current_neighbor = neighbor[pixel];
		if (current_neighbor.is_none()) {
			continue;
		}
		
		GRAPH_EDGE_DIR dir = (*genotype)[current_neighbor.row][current_neighbor.col]; //garbage
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

void Phenotype::build_segment_from_pixel(int row, int col, int segment_id) {
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
			
			add_ingoing_to_active_edge(current_pixel.row, current_pixel.col);	//Kalles mange ganger på samme pixel?

			//Add all neighbors pointing towards the segment

			//Add all neighbors being pointed to by the segment
			GRAPH_EDGE_DIR dir = (*genotype)[current_pixel.row][current_pixel.col];
			add_outgoing_to_active_edge(current_pixel.row, current_pixel.col, dir);

		}

		//All members of the old edge handled. Go on to new edge
		active_edge = new_active_edge;
	}

	segment.push_back(current_segment);
}

void Phenotype::build_segments() {
	std::vector<Segment> segments;
	int segment_id = 0;
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			//Found a pixel that is not a part of a segment
			if (!is_part_of_segment[row][col]) {
				segment_id++;
				build_segment_from_pixel(row, col, segment_id);
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

Phenotype::Phenotype(const Genotype& genotype): Phenotype() {
	this->genotype = &genotype;
	build_segments();
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
	std::cout << ", " << overall_deviation << std::endl;
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
	std::cout << edge_value;
}

void Phenotype::print_segments() {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			std::cout << belongs_to_segment[row][col] << '\t';
		}
		std::cout << std::endl;
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
	const GRAPH_EDGE_DIR* p2_ptr = p2[0];
	GRAPH_EDGE_DIR* c1_ptr = c1[0];
	GRAPH_EDGE_DIR* c2_ptr = c2[0];

	int crossover_point2 = rand() % N_PIXELS;
	int crossover_point1 = rand() % crossover_point2;

	for (int i = 0; i < crossover_point1; i++) {
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
	}

	for (int i = crossover_point1; i < crossover_point2; i++) {
		c1_ptr[i] = p2_ptr[i];
		c2_ptr[i] = (*edge)[i];
	}

	for (int i = crossover_point2; i < N_PIXELS; i++) {
		c1_ptr[i] = (*edge)[i];
		c2_ptr[i] = p2_ptr[i];
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
	for (int i = 0; i < 4; i++) {
		Index new_root = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
		edge[new_root.row][new_root.col] = NONE;
	}
}
/********************************************************************************/ 