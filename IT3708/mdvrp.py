from math import sqrt
#import matplotlib.pyplot as plt
from random import randrange


class Position:
    x = 0
    y = 0

    def __init__(self, x, y):
        self.x = x
        self.y = y


class Data:
    #General data
    n_vehicles = 0
    n_customers = 0
    n_depots = 0

    depot_data = []
    depot_positions = []
    customer_data = []
    customer_positions = []
    distances = []
    distances_depot = []

    def __init__(self, file_name):

        with open(file_name, 'r') as f:
            line = f.readline()
            line = map(int, line.split())

            # Store the general data
            self.n_vehicles = line[0]
            self.n_customers = line[1]
            self.n_depots = line[2]
            self.distances = [[0 for x in range(self.n_customers)] for y in range(self.n_customers)]
            self.distances_depot = [[0 for x in range(self.n_customers)] for y in range(self.n_depots)]

            # Store the depot specific data
            for t in range(self.n_depots):
                line = f.readline()
                line = map(int, line.split())
                self.depot_data.append( (line[0], line[1]) )

            #Store the customer specific data
            for n in range(self.n_customers):
                line = f.readline()
                line = map(int, line.split())

                self.customer_positions.append(Position(line[1], line[2]))
                self.customer_data.append((line[3], line[4]))

            #Store depot positions
            for t in range(self.n_depots):
                line = f.readline()
                line = map(int, line.split())
                self.depot_positions.append(Position(line[1], line[2]))

        #Calculate distance matrix
        for n_row in range(self.n_customers):
            for n_col in range(n_row):
                self.distances[n_row][n_col] = dist(self.customer_positions[n_row], self.customer_positions[n_col])
                self.distances[n_col][n_row] = self.distances[n_row][n_col]

            for depot in range(self.n_depots):
                self.distances_depot[depot][n_row] = dist(self.customer_positions[n_row], self.depot_positions[depot])

# GA classes
class Genotype:
    depot_nbr = 0
    car_nbr = 0
    route = []

    def __init__(self, car_nbr, depot_nbr, route):
        self.depot_nbr = depot_nbr
        self.car_nbr = car_nbr
        self.route = list(route)

    def _feasible(self, total_distance, max_distance, capacity, max_capacity):
        return total_distance < max_distance and capacity < max_capacity

# A single solution to the the problem
class Phenotype:
    routes = []
    def __init__(self, n_depots, n_cars):
        pass

    @classmethod
    def greedy_init(self, data):

        # Group customers to their closest depot
        prev_distance = 9999999
        closest_depot = 0
        grouped_customers = [[] for x in range(data.n_depots)]
        for customer in range(data.n_customers):
            for depot in range(data.n_depots):
                current_distance = data.distances_depot[depot][customer]
                if current_distance < prev_distance:
                    closest_depot = depot
                    prev_distance = current_distance

            grouped_customers[closest_depot].append(customer)

        # Initialize population through a greedy approach
        for depot in range(data.n_depots):
            for vehicle in range(data.n_vehicles):
                leftover_duration = data.depot_data[depot][0]
                leftover_capacity = data.depot_data[depot][1]

                if leftover_duration == 0:
                    leftover_duration = 9999999
                if leftover_capacity == 0:
                    leftover_capacity = 9999999

                route = [0]
                    i = randrange(0, len(grouped_customers[depot]))
                    route.append(grouped_customers[depot][i])
                    del grouped_customers[depot][i]

                    leftover_duration -= data.customer_data[customer][0] # duration from driving distance?
                    leftover_capacity -= data.customer_data[customer][1] ## FIX THIS
                route.append(0)
                self.routes[depot][vehicle] = Genotype(vehicle, depot, route)

    def fitness(self, data):
        # Calculate total route distance
        total_distance = 0
        previous_customer = 0
        for customer in self.route[1:]:
            total_distance += data.distances[customer][previous_customer]
            previous_customer = customer

        # Penalize infeasibility
        # Put in another function
        depot_data = data.depot_data[self.depot_nbr]
        max_distance = depot_data[0]
        max_capacity = depot_data[1]
        capacity = 0
        if not self._feasible(total_distance, max_distance, capacity, max_capacity):
            total_distance += 100

        return total_distance

class GeneticAlgorithm:
    # A list of individual solutions to the problem (phenotypes)
    population = []

    def __init__(self, data, population_size):
        for individual in range(population_size):
            self.population.append(Phenotype.greedy_init(data))

## Util
def dist(pos_a, pos_b):
    return sqrt((pos_a.x-pos_b.x)**2 + (pos_a.y-pos_b.y)**2)

#####  MAIN  #####
# Read data
path = "Testing Data/Data Files/"
file_name = "p01"
data = Data(path + file_name)

## Run the genetic algorithm
population = 1000
mutation_rate = 0.02
crossover_rate = 0.8
elite_individuals = 0.1

# Initialize population
run = GeneticAlgorithm(data, 1)
pass
# TODO
    # make fitness function
    # make initial population
    # implement mutations
    # implement crossover
    # define phenotype
