#include "artificial_bee.h"
#include "config.h"

Bee::Bee() : time_to_live(ABC_TIME_TO_LIVE) { 
	solution.random_initialization();
	solution.convert_to_phenotype(schedule);
	schedule.calculate_fitness();
}

Bee::Bee(char* gene) : Bee() {
	solution = CHROMOSOME_TYPE(gene);
	solution.convert_to_phenotype(schedule);
	schedule.calculate_fitness();
}

//Assumes phenotypes and solutions are initialized
void Bee::local_search() {
	CHROMOSOME_TYPE new_solution = solution;
	Phenotype new_schedule = schedule;

	//Mutate solution a number of times or until a better solution is found
	for (int i = 0; i < ABC_LOCAL_SEARCH_MAX_MUTATIONS; i++) {

		//Pick a random mutation operation
		MUTATION_OPERATIONS mutation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
		new_solution.mutate(mutation);

		//Calculate the new fitness
		new_solution.convert_to_phenotype(new_schedule);
		new_schedule.calculate_fitness();

		//Update solution if it is better and stop searching
		if (new_schedule <= schedule) {
			//std::cout << "New solution found\n";
			if (schedule.genotype_mismatch()) {
				schedule.convert_to_genotype(solution);
			}

			schedule = new_schedule;
			solution = new_solution;
			
			time_to_live = ABC_TIME_TO_LIVE;
		}
	}

	time_to_live--;
}

//Runs the mutation given by operation and returns a boolean saying if the solution improved or not
bool Bee::search_operation(MUTATION_OPERATIONS operation) {
	CHROMOSOME_TYPE new_solution = solution;
	Phenotype new_schedule = schedule;

	new_solution.mutate(operation);
	new_solution.convert_to_phenotype(new_schedule);
	new_schedule.calculate_fitness();

	if (new_schedule <= schedule) {
		solution = new_solution;
		schedule = new_schedule;
		
		time_to_live = ABC_TIME_TO_LIVE;
		return true;
	}

	time_to_live--;
	return false;
}

Bee Bee::operator = (const Bee& rhs) {
	solution = rhs.solution;
	schedule = rhs.schedule;
	time_to_live = rhs.time_to_live;

	return *this;
}

void Bee::create_scout_bee() {
	for(int i = 0; i < ABC_N_SCOUT_BEE_INSERTIONS; i++) {
		MUTATION_OPERATIONS operation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
		//solution.mutate(INSERT1);
		solution.mutate(operation);
	}

	solution.convert_to_phenotype(schedule);
	schedule.calculate_fitness();

	time_to_live = ABC_TIME_TO_LIVE;
}


Hive::Hive() {
	employed_bees.resize(ABC_POPULATION);
	adaptive_list.reserve(ABC_ADAPTIVE_LIST_SIZE);

	int best_bee = INF;
	int elite_bee_index = 0;

	//Initialize the adpative list
	for (int i = 0; i < ABC_ADAPTIVE_LIST_SIZE; i++) {
		MUTATION_OPERATIONS operation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
		adaptive_list.push_back(operation);
	}

	//Find the best solution
	for (int i = 0; i < ABC_POPULATION; i++) {
		if (employed_bees[i].get_nectar_amount() < best_bee) {
			elite_bee_index = i;
			best_bee = employed_bees[i].get_nectar_amount();
		}
	}

	elite_bee = employed_bees[elite_bee_index];
}

void Hive::employed_bee_phase() {
	for (int bee = 0; bee < ABC_POPULATION; bee++) {
		_search_for_new_sources(&employed_bees[bee]);
		
		//New elite found
		if (employed_bees[bee] <= elite_bee) {
			elite_bee = employed_bees[bee];
		}
	}
}

void Hive::onlooker_bee_phase() {
	for (int i = 0; i < ABC_POPULATION; i++) {
		Bee* onlooker_bee = _tournament_selection();
		_search_for_new_sources(onlooker_bee);

		if (onlooker_bee->get_remaining_time_to_live() == 0) {
			old_bee.push_back(onlooker_bee);
		}
	}
}

void Hive::scout_bee_phase() {
	for (int i = 0; i < ABC_SCOUT_BEE_POPULATION; i++) {
		if (old_bee.empty()) {
			return;
		}

		Bee* scout_bee = old_bee.back();
		old_bee.pop_back();

		*scout_bee = elite_bee;
		scout_bee->create_scout_bee();
	}
}

//Will either do several mutations in a local search or pick a mutation from the adaptive list
void Hive::_search_for_new_sources(Bee* bee) {
	bool do_local_search = (rand() % 100) <= ABC_LOCAL_SEARCH_CHANCE * 100;
	//There are two ways of searching
	if (do_local_search) {
		bee->local_search();
	}
	else {
		_run_adaptive_list_operation(bee);
	}
}

void Hive::_refill_adaptive_list() {

	while(adaptive_list.size() < ABC_ADAPTIVE_LIST_SIZE) {
		bool from_winning_list = (rand() % 100) < ABC_WINNING_LIST_CHANCE * 100;

		if (from_winning_list && !successful_mutations.empty()) {
			MUTATION_OPERATIONS new_operation = successful_mutations.back();
			adaptive_list.push_back(new_operation);
			successful_mutations.pop_back();

		}
		//else if (successful_mutations.empty()) {
		//	std::cout << "Winning list empty!\n";
		//}

		else {
			MUTATION_OPERATIONS new_operation = static_cast<MUTATION_OPERATIONS>(rand() % N_MUTATIONS);
			adaptive_list.push_back(new_operation);
		}

	}

	successful_mutations.clear();
}

void Hive::_run_adaptive_list_operation(Bee* bee) {
	MUTATION_OPERATIONS operation = adaptive_list.back();
	bool solution_improved = bee->search_operation(operation);

	adaptive_list.pop_back();
	
	if (solution_improved) {
		successful_mutations.push_back(operation);
	}

	if (adaptive_list.empty()) {
		_refill_adaptive_list();
	}


}

Bee* Hive::_tournament_selection() {
	int i = rand() % employed_bees.size();
	Bee* best_bee = &employed_bees[i];

	for (int t = 0; t < ABC_TOURNAMENT_SIZE; t++) {
		int i = rand() % employed_bees.size();
		if (*best_bee > employed_bees[i]) {
			best_bee = &employed_bees[i];
		}
	}
	
	return best_bee;
}

void Hive::run_optimization(int optimal_value) {
	for (int i = 0; i < ABC_MAX_GENERATIONS && elite_bee.get_nectar_amount() > optimal_value; i++) {
		employed_bee_phase();
		onlooker_bee_phase();
		scout_bee_phase();
		std::cout << "Best estimate for generation " << i << ": " << elite_bee.get_nectar_amount() << std::endl;
	}

	std::cout << "Best estimate: " << elite_bee.get_nectar_amount() << std::endl;
	elite_bee.print();
	elite_bee.store_solution(CSV_DIR);
}

std::ostream& operator << (std::ostream& out, const Bee& bee) {
	out << bee.solution;
	return out;
}