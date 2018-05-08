#pragma once
#define INF 999999
#define OPTIMAL_VALUE 1130
#define TEST_DIR "../Test Data/2.txt"
#define CHROMOSOME_TYPE NewRep				//Chosen chromosome representation
//#define CHROMOSOME_TYPE OttoRep

//Artificial bee colony
#define ABC_POPULATION 500
#define ABC_SCOUT_BEE_POPULATION 1			//Mutated elite
#define ABC_N_SCOUT_BEE_INSERTIONS 3		//Number of mutations when copying elite
#define ABC_LOCAL_SEARCH_MAX_MUTATIONS 10	//Number of mutations done in local search
#define ABC_TIME_TO_LIVE 100				//Time before a solution is considered old
#define ABC_ADAPTIVE_LIST_SIZE 100			//Length of list of mutations
#define ABC_LOCAL_SEARCH_CHANCE 0.2			//The chance of doing a local search instead of an adaptive action
#define ABC_WINNING_LIST_CHANCE 0.75		//Percentage of mutations taken from the successful list
#define ABC_TOURNAMENT_SIZE 2
#define ABC_MAX_GENERATIONS 1000

//Ant colony
#define ACO_INITIAL_PHEROMONE_STRENGTH 10
#define ACO_FITNESS_PHEROMONE_CONTRIBUTION 5000
#define ACO_MAX_PHEROMONE_STRENGTH 50
#define ACO_MIN_PHEROMONE_STRENGTH 5
#define ACO_EVAPORATION_RATE 0.95
#define ACO_HEURISTIC_WEIGHTING 0.4
#define ACO_PHEROMONE_WEIGHTING 0.6
#define ACO_MAX_GENERATIONS 70
#define ACO_POPULATION 200

//ACO additions
#define ACO_TOURNAMENT_SIZE 5
#define ACO_N_LEADER_ANTS 10
#define ACO_LOCAL_SEARCH_MAX_MUTATIONS 10
#define ACO_LOCAL_SEARCH_CHANCE 0.2
#define ACO_MUTATION_CHANCE 0.1
