from math import sqrt
#import matplotlib.pyplot as plt
from random import shuffle


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



# A single solution to the the problem
class Phenotype:
    routes = []
    chromosome = []
    constraint_violations = 0

    def __init__(self, n_depots, n_vehicles):
        #self.routes = [[[] for x in range(n_vehicles) ] for y in range(n_depots)]
        self.routes = [[] for x in range(n_depots)]
        self.chromosome = [None for x in range(n_depots)]

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
            shuffle(grouped_customers[depot])

            while len(grouped_customers[depot]) > 0:
                for vehicle in range(data.n_vehicles):
                    self.routes[depot][vehicle].append(grouped_customers[depot].pop())

                    if len(grouped_customers[depot]) == 0:
                        break

    def permuted_init(self, data):
        # Group customers to their closest depot
        n = data.n_customers // data.n_depots
        customers = list(range(1, data.n_customers+1))
        shuffle(customers)

        # Award n_customers / n_depots customers to each depot
        for depots in range(data.n_depots-1):
            self.chromosome[depots] = list(customers[-n:])
            del customers[-n:]

        # In case the amount of customers are not divisible by n_depots
        self.chromosome[-1] = list(customers)

    def schedule_routes(self, data):
        for depot in range(data.n_depots):
            duration_limit = data.depot_data[depot][0]
            demand_limit = data.depot_data[depot][1]
            total_duration = 0
            total_demand = 0

            if duration_limit == 0:
                duration_limit = 9999999

            for i in range(len(self.chromosome)):
                customer = self.chromosome[depot][i]-1
                total_duration += data.customer_data[customer][0]
                total_demand += data.customer_data[customer][1]

                if total_duration > duration_limit or total_demand > demand_limit:
                    self.routes[depot].append(i - 1)  # store the index of the last customer in the route

            # Check constraint violations
            if data.n_vehicles < len(self.routes[depot]):
                self.constraint_violations += 1

    def determine_constraint_violations(self, data):
        for depot in range(data.n_depots):
            duration_limit = data.depot_data[depot][0]
            demand_limit = data.depot_data[depot][1]
            total_duration = 0
            total_demand = 0
            constraint_violations = 0

            if duration_limit == 0:
                duration_limit = 9999999

            for i in range(len(self.chromosome)):
                customer = self.chromosome[depot][i]-1
                total_duration += data.customer_data[customer][0]
                total_demand += data.customer_data[customer][1]

            if total_duration > duration_limit or total_demand > demand_limit:
                constraint_violations += 1

        return constraint_violations

    def fitness_with_check(self, data):
        # Calculate total route distance for every vehicle
        total_distance = 0
        for depot in range(data.n_depots):
            for vehicle in range(data.n_vehicles):
                # Distance from depot to customer
                prev_customer = self.routes[depot][vehicle][0]
                route_distance = data.distances_depot[depot][prev_customer]
                route_duration = data.customer_data[prev_customer][0]
                route_demand = data.customer_data[prev_customer][1]

                for customer in self.routes[depot][vehicle][1:]:
                    route_distance += data.distance[customer][prev_customer]
                    prev_customer = customer
                    route_duration += data.customer_data[customer][0]
                    route_demand += data.customer_data[customer][1]

                # Penalize exceeding of constraints
                if route_duration > data.depot_data[depot][0] and data.depot_data[depot][0] != 0:
                    route_distance += 100

                if route_demand > data.depot_data[depot][1] and data.depot_data[depot][1] != 0:
                    route_distance += 100

                route_distance += data.distances_depot[depot][prev_customer]
                total_distance += route_distance

        return total_distance

    def fitness(self, data):
        # Calculate total route distance for every vehicle
        total_distance = 0
        start = 0
        for depot in range(data.n_depots):

            # Distance from depot to customer
            prev_customer = self.chromosome[depot][start]
            route_distance = data.distances_depot[depot][prev_customer]

            # Distance between customers
            stop = self.routes[depot]
            for customer in self.chromosome[depot][start:stop]:
                route_distance += data.distance[customer][prev_customer]
                prev_customer = customer


            total_distance += route_distance + data.distances_depot[depot][prev_customer]

            start = stop + 1

        return total_distance + self.constraint_violations*100

    def _mutate(self):
        pass

    def _recombine(self):
        pass

class GeneticAlgorithm:
    # A list of individual solutions to the problem (phenotypes)
    population = []

    def __init__(self, data, population_size):
        for individual in range(population_size):
            self.population.append(Phenotype(data.n_depots, data.n_vehicles))
            self.population[-1].permuted_init(data)

    def create_next_generation(self):
        pass

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
elitism_rate = 0.1

# Initialize population
run = GeneticAlgorithm(data, 3)

for i in range(3):
    run.population[i].schedule_routes(data)
    print(run.population[i].fitness(data))
