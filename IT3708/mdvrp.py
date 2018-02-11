from math import sqrt
#import matplotlib.pyplot as plt
from random import shuffle
from random import randint
from random import random
from random import choice
from copy import deepcopy
from math import floor
from numpy.random import choice as npChoice


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
    chromosome = []
    fitness = 0

    def __init__(self, n_depots, n_vehicles):
        #self.routes = [[[] for x in range(n_vehicles) ] for y in range(n_depots)]
        self.chromosome = [[[] for y in range(data.n_vehicles)] for x in range(n_depots)]

    def permuted_init(self, data):
        # Group customers to their closest depot
        n = data.n_customers // data.n_depots # Amount of customers assigned to each depot
        customers = list(range(data.n_customers))  # List of customers
        shuffle(customers)

        # Award n_customers / n_depots customers to each depot
        for depots in range(data.n_depots):
            customers_at_depot = list(customers[-n:])
            del customers[-n:]

            n_customers_at_depot = n // data.n_vehicles
            for vehicles in range(data.n_vehicles):
                self.chromosome[depots][vehicles].extend(list(customers_at_depot[-n_customers_at_depot:]))
                del customers_at_depot[-n_customers_at_depot:]

        # In case the amount of customers are not divisible by n_depots
        self.chromosome[-1][-1].extend(customers)
        pass

    def _constraint_violations(self, data):
        constraint_violations = 0

        for depot in range(data.n_depots):
            duration_limit = data.depot_data[depot][0]
            demand_limit = data.depot_data[depot][1]
            total_duration = 0
            total_demand = 0

            if duration_limit == 0:
                duration_limit = 9999999

            for vehicle in range(data.n_vehicles):
                for customer in self.chromosome[depot][vehicle]:
                    total_duration += data.customer_data[customer][0]
                    total_demand += data.customer_data[customer][1]

                if total_duration > duration_limit or total_demand > demand_limit:
                    constraint_violations += 1

        return constraint_violations

    def calculate_fitness(self, data):
        # Calculate total route distance for every vehicle
        total_distance = 0
        start = 0
        for depot in range(data.n_depots):
            for vehicle in range(data.n_vehicles):
                route = self.chromosome[depot][vehicle]
                total_distance += self._calculate_route_distance(route, depot, data)

        self.fitness = total_distance + self._constraint_violations(data)*100

    def mutate(self, data, mutation_chance):
        p = random()
        if p <= mutation_chance:
            p = random()
            depot = randint(0, data.n_depots - 1)

            if p < 0.333333334:
                self._mutate_intra_swap(data, depot)
                pass
            elif p < 0.666666667:
                self._mutate_reversal(data, depot)
                pass

            else:
                self._mutate_inter_swap(data)
                pass

            return 1
        return 0

    def _mutate_inter_swap(self, data):
        depot1 = randint(0, data.n_depots - 1)
        depot2 = randint(0, data.n_depots - 1)
        vehicle1 = randint(0, data.n_vehicles - 1)
        vehicle2 = randint(0, data.n_vehicles - 1)

        if len(self.chromosome[depot1][vehicle1]) == 0 or len(self.chromosome[depot2][vehicle2]) == 0:
            return

        customer1 = randint(0, len(self.chromosome[depot1][vehicle1]) - 1)
        customer2 = randint(0, len(self.chromosome[depot2][vehicle2]) - 1)

        temp = self.chromosome[depot1][vehicle1][customer1]
        self.chromosome[depot1][vehicle1][customer1] = self.chromosome[depot2][vehicle2][customer2]
        self.chromosome[depot2][vehicle2][customer2] = temp

    def _mutate_intra_swap(self, data, depot):
        vehicle1 = randint(0, data.n_vehicles - 1)
        vehicle2 = randint(0, data.n_vehicles - 1)
        if len(self.chromosome[depot][vehicle1]) == 0 or len(self.chromosome[depot][vehicle2]) == 0:
            return


        customer1 = randint(0, len(self.chromosome[depot][vehicle1]) - 1)
        customer2 = randint(0, len(self.chromosome[depot][vehicle2]) - 1)


        temp = self.chromosome[depot][vehicle1][customer1]
        self.chromosome[depot][vehicle1][customer1] = self.chromosome[depot][vehicle2][customer2]
        self.chromosome[depot][vehicle2][customer2] = temp

    def _mutate_reversal(self, data, depot):
        # Treat depot chromosome as a single strand
        route_length = []
        routes = []
        for route in self.chromosome[depot]:
            route_length.append(len(route))
            routes.extend(route)

        cutpoint_start = randint(1, len(routes))
        cutpoint_stop = randint(cutpoint_start, len(routes))

        # Reverse part of the chromosome
        before = len(routes)
        routes[cutpoint_start:cutpoint_stop] = routes[cutpoint_stop-1:cutpoint_start-1:-1]
        after = len(routes)

        # Split strand into the previous presentation
        start = 0
        for vehicle in range(data.n_vehicles):
            stop = route_length[vehicle]+start

            self.chromosome[depot][vehicle] = routes[start:stop]
            start = stop

    def _calculate_route_distance(self, route, depot, data):
        if len(route) == 0:
            return 0

        # Distance from depot to customer
        route_length = 0
        prev_customer = route[0]
        for customer in route[1:]:

            # Distance between customers
            route_length += data.distances[customer][prev_customer]
            prev_customer = customer

        depot_distance = data.distances_depot[depot][route[0]] + data.distances_depot[depot][route[-1]]
        return route_length + depot_distance

    def calculate_best_insertion(self, route, depot, customer, data):
        if len(route) == 0:
            return data.distances_depot[depot][customer]*2, 0

        # Insert customer into every possible location
        shortest_length = data.distances_depot[depot][customer] + data.distances[customer][route[0]]
        shortest_length_index = 0

        for i in range(1, len(route)-1):
            next_customer = route[i+1]
            prev_customer = route[i-1]
            added_length = data.distances[prev_customer][customer] + data.distances[customer][next_customer]

            if added_length < shortest_length:
                shortest_length = added_length
                shortest_length_index = i

        # See if appending is best
        last_length = data.distances[route[-1]][customer] + data.distances_depot[depot][customer]
        if last_length < shortest_length:
            shortest_length = last_length
            shortest_length_index = len(route)

        return shortest_length, shortest_length_index

    def _remove_customers(self, route, data):


    def recombine(self, p2, data):
        depot_target = randint(0, data.n_depots - 1)
        vehicle1 = randint(0, data.n_vehicles - 1)
        vehicle2 = randint(0, data.n_vehicles - 1)

        route1 = list(self.chromosome[depot_target][vehicle1])
        route2 = list(p2.chromosome[depot_target][vehicle2])

        c1 = deepcopy(self)
        c2 = deepcopy(p2)

        # Remove customer of the chosen crossover routes from the children
        for depot in range(data.n_depots):
            for vehicle in range(data.n_vehicles):

                # Child 1
                customer = 0
                while customer < len(c1.chromosome[depot][vehicle]):
                    if c1.chromosome[depot][vehicle][customer] in route2:
                        del c1.chromosome[depot][vehicle][customer]
                        customer -= 1

                    customer += 1

                # Child 2
                customer = 0
                while customer < len(c2.chromosome[depot][vehicle]):
                    if c2.chromosome[depot][vehicle] in route1:
                        del c2.chromosome[depot][vehicle][customer]
                        customer -= 1

                    customer += 1

        # Insert customers where route distance is minimized
        min_distance = 999999999
        min_index1 = [0 for i in range(len(route2))]
        min_index2 = [0 for i in range(len(route1))]
        insertion_vehicle1 = [0 for i in range(len(route2))]
        insertion_vehicle2 = [0 for i in range(len(route1))]
        for vehicle in range(data.n_vehicles):

            insertion_route = c1.chromosome[depot_target][vehicle]
            for customer in range(len(route2)):

                distance, index = c1.calculate_best_insertion(insertion_route, depot_target, route2[customer], data)
                if distance < min_distance:
                    min_distance = distance
                    insertion_vehicle1[customer] = vehicle
                    min_index1[customer] = index

            insertion_route = c2.chromosome[depot_target][vehicle]
            for customer in range(len(route1)):

                distance, index = c2.calculate_best_insertion(insertion_route, depot_target, route1[customer], data)
                if distance < min_distance:
                    min_distance = distance
                    insertion_vehicle2[customer] = vehicle
                    min_index2[customer] = index

        for customer in range(len(route2)):
            c1.chromosome[depot_target][insertion_vehicle1[customer]].insert(min_index1[customer], route2[customer])

        for customer in range(len(route1)):
            c2.chromosome[depot_target][insertion_vehicle2[customer]].insert(min_index2[customer], route1[customer])

        return c1, c2

    def __lt__(self, rhs):
        return self.fitness < rhs.fitness

    def __le__(self, rhs):
        return self.fitness <= rhs.fitness

    def __eq__(self, rhs):
        return self.fitness == rhs.fitness

    def __ne__(self, rhs):
        return self.fitness != rhs.fitness

    def __gt__(self, rhs):
        return self.fitness > rhs.fitness

    def __ge__(self, rhs):
        return self.fitness >= rhs.fitness


class GeneticAlgorithm:
    # A list of individual solutions to the problem (phenotypes)
    population = []

    def __init__(self, data, population_size):
        for individual in range(population_size):
            self.population.append(Phenotype(data.n_depots, data.n_vehicles))
            self.population[-1].permuted_init(data)

    # Can select same parent multiple times.
    def selection(self, elite_percentage, population_size, data):
        sorted_population = []
        parents = []

        n_elites = int(floor(len(self.population) * elite_percentage))

        # Sort population by fitness
        for individual in range(len(self.population)):
            self.population[individual].calculate_fitness(data)
            insert_into(sorted_population, self.population[individual])
        elites = sorted_population[:n_elites]

        # Rank selection
        normalize = len(sorted_population) * (len(sorted_population)-1) / 2.0
        weights = [(len(sorted_population) - i - 1)/normalize for i in range(len(sorted_population))]

        while len(parents) + len(elites) < population_size:
            individual = npChoice(sorted_population, p=weights)
            parents.append(individual)

        return parents, elites


    def create_next_generation(self, data, mutation_chance, elite_percentage, population_size):
        parents, elites = self.selection(elite_percentage, population_size, data)
        child_population = elites
        # Pick elite individuals

        for individual in parents:
            p2 = choice(parents)
            c1, c2 = individual.recombine(p2, data)
            c1.mutate(data, mutation_chance)
            c2.mutate(data, mutation_chance)
            child_population.extend([c1, c2])

        self.population = child_population




## Util
def dist(pos_a, pos_b):
    return sqrt((pos_a.x-pos_b.x)**2 + (pos_a.y-pos_b.y)**2)
def insert_into(list_a, element):
    if len(list_a) == 0:
        list_a.append(element)

    for i in range(len(list_a)):
        if element > list_a[i]:
            list_a.insert(i, element)
            return


#####  MAIN  #####
# Read data
path = "Testing Data/Data Files/"
file_name = "p01"
data = Data(path + file_name)

## Run the genetic algorithm
n_generations = 200
population = 400
mutation_rate = 0.02
crossover_rate = 0.2
elitism_rate = 0.1
parent_percentage = 0.3

# Initialize population
run = GeneticAlgorithm(data, population)
for generation in range(n_generations):
    print generation
    run.create_next_generation(data, mutation_rate, elitism_rate, population)

## TODO fix selection so that a parent is only chosen once
## TODO make sure population doesn't die out
