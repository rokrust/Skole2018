#include "Genetic_algorithm.hpp"
#include <iterator>
#include <math.h>
#include <limits>
#include <array>
#include <chrono>
#include <thread>
/********************************** Population **********************************/
Population::Population(int population_size, int archive_size, double mutation_rate, double crossover_rate): 
	archive_size(archive_size), mutation_rate(mutation_rate), crossover_rate(crossover_rate) {
	population.reserve(population_size);
	
	std::vector<std::thread> thread_vec;
	for (int i = 0; i < population_size; i++) {
		population.push_back(Genotype());
		Index root_node = { rand() % IMAGE_HEIGHT, rand() % IMAGE_WIDTH };
		thread_vec.push_back(std::thread(&MST::genotype_generator, MST(), std::ref(population[i]), root_node));
	}

	for (int i = 0; i < population_size; i++) {
		thread_vec[i].join();
	}

}
/********************************************************************************/

/*********************************** Phenotype **********************************/
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

void Phenotype::build_segment_from_pixel(int row, int col) {
	active_edge.push_back({ row, col });
	is_part_of_segment[row][col] = true;
	Segment current_segment;
	
	static int segment_id = 0;
	segment_id++;

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
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			
			//Found a pixel that is not a part of a segment
			if (!is_part_of_segment[row][col]) {
				build_segment_from_pixel(row, col);
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

double Phenotype::calculate_overall_deviation() {
	double overall_deviation = 0;

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

	return overall_deviation;
}

double Phenotype::calculate_edge_value() {
	double edge_value = 0.0;

	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			std::array<Index, 4> neighbor = {};

			image.get_neighbors(row, col, neighbor);
			Index current_pixel = { row, col };
			int current_pixel_id = belongs_to_segment[current_pixel.row][current_pixel.col];

			for (int i = 0; i < 4; i++) {
				Index current_neighbor = neighbor[i];
				if (current_neighbor.is_none()) { continue; }
				
				int current_neighbor_id = belongs_to_segment[current_neighbor.row][current_neighbor.col];

				if (current_neighbor_id != current_pixel_id) {
					edge_value += image.distance_between(current_pixel, current_neighbor);
				}
			}
		}
	}

	return edge_value;
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
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->in_tree[row][col] = original.in_tree[row][col];
		}
	}

	this->mst_set = original.mst_set;
	this->priority_queue = original.priority_queue;
}

MST& MST::operator=(const MST &rhs) {
	for (int row = 0; row < IMAGE_HEIGHT; row++) {
		for (int col = 0; col < IMAGE_WIDTH; col++) {
			this->in_tree[row][col] = rhs.in_tree[row][col];
		}
	}

	this->mst_set = rhs.mst_set;
	this->priority_queue = rhs.priority_queue;

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
/********************************************************************************/ 