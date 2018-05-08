#include "ant_colony.h"
#include "data.h"
#include "util.h"
#include <vector>
#include <iostream>


Path::Path() {
	_pheromone_strength = ACO_INITIAL_PHEROMONE_STRENGTH;

}

void Path::_saturate_pheromone_strength() {
	if (_pheromone_strength < ACO_MIN_PHEROMONE_STRENGTH) {
		_pheromone_strength = ACO_MIN_PHEROMONE_STRENGTH;
	}
	else if (_pheromone_strength > ACO_MAX_PHEROMONE_STRENGTH) {
		_pheromone_strength = ACO_MAX_PHEROMONE_STRENGTH;
	}

}

Path Path::operator*=(double w) {
	_pheromone_strength *= w;
	_saturate_pheromone_strength();

	return *this;
}

Path Path::operator+(double w) {
	Path copy = *this;
	copy += w;
	copy._saturate_pheromone_strength();

	return copy;
}

Path Path::operator+=(double w) {
	_pheromone_strength += w;
	_saturate_pheromone_strength();

	return *this;
}

Path Path::operator=(double w) {
	_pheromone_strength = w;
	return *this;
}



Map::Map() {
	_allocate_data();

	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			for (int k = 0; k < data.N_JOBS; k++) {
				pheromone_trail[i][j][k] = ACO_INITIAL_PHEROMONE_STRENGTH;
			}
		}
	}

	for (int i = 0; i < data.N_JOBS; i++) {
		start_node[i] = ACO_INITIAL_PHEROMONE_STRENGTH;
	}
}

Map::Map(const Map& map) {
	_allocate_data();

	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			for (int k = 0; k < data.N_JOBS; k++) {
				pheromone_trail[i][j][k] = map.pheromone_trail[i][j][k];
			}
		}
	}

	for (int i = 0; i < data.N_JOBS; i++) {
		start_node[i] = map.start_node[i];
	}
}

void Map::_allocate_data() {
	start_node = new Path[data.N_JOBS];
	pheromone_trail = new Path**[data.N_JOBS*data.N_MACHINES];
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		pheromone_trail[i] = new Path*[data.N_JOBS];

		for (int j = 0; j < data.N_JOBS; j++) {
			pheromone_trail[i][j] = new Path[data.N_JOBS];
		}
	}
}

Map::~Map() {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			delete[] pheromone_trail[i][j];
		}
		delete[] pheromone_trail[i];
	}

	delete[] pheromone_trail;
	delete[] start_node;
}

int Map::_find_next_path(int layer, int from, std::vector<int> excluded_paths) {
	std::vector<double> path_pheromone_strength;
	
	for (int to = 0; to < data.N_JOBS; to++) {
		double p = get_path_pheromone_strength(layer, from, to);
		
		//Excluded paths are weighted by zero and will not be chosen
		for (int i = 0; i < excluded_paths.size(); i++) {
			if (excluded_paths[i] == to) {
				p = 0;
				break;
			}
		}

		path_pheromone_strength.push_back(p);
	}
	
	int next_job = util::weighted_rand(path_pheromone_strength);
	return next_job;
}

void Map::find_new_trail(Ant& ant) {
	std::vector<int> excluded_paths;
	std::vector<int> remaining_jobs(data.N_JOBS, data.N_MACHINES);
	int previous_job = _find_next_path(0, 0, excluded_paths); //Set the first job

	for (int layer = 0; layer < data.N_JOBS*data.N_MACHINES; layer++) {
		int next_job = _find_next_path(layer, previous_job, excluded_paths);
		ant.set_job(layer, next_job);

		//Job is finished. Exclude from the rest of the path calculations
		if (--remaining_jobs[next_job] == 0) {
			excluded_paths.push_back(next_job);
		}
	}

	ant.search();
}

double Map::_get_heuristic_weight(int layer, int from, int to) {
	
}

void Map::evaporate_pheromone_trail() {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			for (int k = 0; k < data.N_JOBS; k++) {
				pheromone_trail[i][j][k] *= ACO_EVAPORATION_RATE;
			}
		}
	}
}

void Map::update_pheromone_trail(const Ant& ant) {
	double pheromone_strength = ant.get_pheromone_strength();
	
	start_node[ant.get_job(0)] += pheromone_strength;
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES - 1; i++) {
		int from = ant.get_job(i);
		int to = ant.get_job(i + 1);

		pheromone_trail[i][from][to] += pheromone_strength;
		
	}
}

double Map::get_path_pheromone_strength(int layer, int from, int to) {
	//double heuristic_weight = _get_heuristic_weight(layer, from, to);

	if (layer == 0) {
		return start_node[to].get_pheromone_strength();
	}

	return pheromone_trail[layer][from][to].get_pheromone_strength();
}

Map Map::operator=(const Map& map) {
	for (int i = 0; i < data.N_JOBS*data.N_MACHINES; i++) {
		for (int j = 0; j < data.N_JOBS; j++) {
			pheromone_trail[i][j] = map.pheromone_trail[i][j];
		}
	}

	return *this;
}

std::ostream& operator<<(std::ostream& out, const Map& map) {
	for (int layer = 0; layer < data.N_JOBS*data.N_MACHINES; layer++) {
		out << "Layer " << layer << ":\n";

		for (int from = 0; from < data.N_JOBS; from++) {
			for (int to = 0; to < data.N_JOBS; to++) {
				out << '\t' << map.pheromone_trail[layer][from][to].get_pheromone_strength();
			}
			out << '\n';
		}
		out << '\n';
	}

	return out;
}




Ant::Ant() {

}

Ant::Ant(const Ant& ant) {
	solution = ant.solution;
	schedule = ant.schedule;
}

void Ant::calculate_pheromone_contribution() {
	solution.convert_to_phenotype(schedule);
	schedule.calculate_fitness();
}

Ant Ant::operator=(const Ant& rhs) {
	solution = rhs.solution;
	schedule = rhs.schedule;

	return *this;
}

void Ant::local_search() {
	CHROMOSOME_TYPE new_solution = solution;
	Phenotype new_schedule = schedule;

	//Mutate solution a number of times or until a better solution is found
	for (int i = 0; i < ACO_LOCAL_SEARCH_MAX_MUTATIONS; i++) {

		//Pick a random mutation operation
		MUTATION_OPERATIONS mutation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
		new_solution.mutate(mutation);

		//Calculate the new fitness
		new_solution.convert_to_phenotype(new_schedule);
		new_schedule.calculate_fitness();

		//Update solution if it is better and stop searching
		if (new_schedule <= schedule) {
			schedule = new_schedule;
			solution = new_solution;

		}
	}

}

void Ant::search() {
	double p = (rand() % 100) / 100.0;
	if (p < ACO_LOCAL_SEARCH_CHANCE) {
		local_search();
	}
	else if (p < ACO_LOCAL_SEARCH_CHANCE + ACO_MUTATION_CHANCE) {
		MUTATION_OPERATIONS mutation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
		mutate(mutation);
	}
}




AntColony::AntColony() {
	population.resize(ACO_POPULATION);
	Ant ant;
	Map map;
	map.find_new_trail(ant);
	ant.calculate_pheromone_contribution();
	elite_ants.push_back(ant);
}

AntColony::AntColony(const AntColony& colony) {
}

void AntColony::build_trails() {
	for (int i = 0; i < ACO_POPULATION; i++) {
		Ant& ant = population[i];
		map.find_new_trail(ant);
		ant.calculate_pheromone_contribution();

		//Copy all time best ants
		if (ant == elite_ants.back()) {
			elite_ants.push_back(ant);
		}
		else if (ant > elite_ants.back()) {
			elite_ants.clear();
			elite_ants.push_back(ant);
		}
	}
}

void AntColony::update_trails() {
	map.evaporate_pheromone_trail();
	
	for (int i = 0; i < elite_ants.size(); i++) {
		std::cout << "Number of elites: " << elite_ants.size() << std::endl;
		map.update_pheromone_trail(elite_ants[i]);
	}
	
	/*
	for (int i = 0; i < ACO_N_LEADER_ANTS; i++) {
		Ant* leader = _tournament_selection();
		map.update_pheromone_trail(*leader);
	}
	*/

}

void AntColony::run_optimization(int optimal_value) {
	int best_estimate = INF;
	for (int i = 0; i < ACO_MAX_GENERATIONS && best_estimate > optimal_value; i++) {
		build_trails();
		update_trails();
		
		best_estimate = elite_ants.back().get_fitness();
		std::cout << "Best estimate for generation " << i << ": " << best_estimate << std::endl;
	}
	std::cout << map << std::endl;
	std::cout << "Best estimate: " << best_estimate << std::endl;
}

Ant* AntColony::_tournament_selection() {
	int i = rand() % population.size();
	Ant* best_ant = &population[i];

	for (int t = 0; t < ACO_TOURNAMENT_SIZE; t++) {
		int i = rand() % population.size();
		if (*best_ant > population[i]) {
			best_ant = &population[i];
		}
	}

	return best_ant;
}

AntColony AntColony::operator=(const AntColony& colony) {
	map = colony.map;

	return *this;
}
