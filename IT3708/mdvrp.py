from math import sqrt
#import matplotlib.pyplot as plt
from random import shuffle
from random import randint
from random import random
from random import choice
from copy import deepcopy
from math import floor
import matplotlib.pyplot as plt


class Position:
    x = 0
    y = 0

    def __init__(self, x, y):
        self.x = x
        self.y = y

class Customer:
    duration = 0
    load = 0
    position = None

    def __init__(self, position, duration, load):
        self.position = position
        self.duration = duration
        self.load = load

class Depot:
    duration_limit = 0
    load_limit = 0
    position = None

    def __init__(self, duration_limit, load_limit):
        self.duration_limit = duration_limit
        self.load_limit = load_limit

class Data:
    #General data
    n_vehicles = 0
    n_customers = 0
    n_depots = 0

    depot = []
    customer = []

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
            self._store_depot_data(f)

            #Store the customer specific data
            self._store_customer_data(f)

            #Store depot positions
            self._store_depot_positions(f)

        #Calculate distance matrix
        self._calculate_distances()

    def _store_customer_data(self, file):
        for n in range(self.n_customers):
            line = file.readline()
            line = map(int, line.split())

            customer = Customer(Position(line[1], line[2]), line[3], line[4])
            self.customer.append(customer)
    def _store_depot_data(self, file):
        for t in range(self.n_depots):
            line = file.readline()
            line = map(int, line.split())

            if line[0] == 0:
                line[0] = 9999999999

            self.depot.append(Depot(line[0], line[1]))
    def _store_depot_positions(self, file):
        for t in range(self.n_depots):
            line = file.readline()
            line = map(int, line.split())
            self.depot[t].position = Position(line[1], line[2])
    def _calculate_distances(self):
        for n_row in range(self.n_customers):
            for n_col in range(n_row):
                self.distances[n_row][n_col] = self._dist(self.customer[n_row].position, self.customer[n_col].position)
                self.distances[n_col][n_row] = self.distances[n_row][n_col]

            for depot in range(self.n_depots):
                self.distances_depot[depot][n_row] = self._dist(self.customer[n_row].position, self.depot[depot].position)

    def _dist(self, pos_a, pos_b):
        return sqrt((pos_a.x - pos_b.x) ** 2 + (pos_a.y - pos_b.y) ** 2)



# A single solution to the the problem
class Phenotype:
    chromosome = []
    route = []
    fitness = 0
    total_duration = []
    total_load = []

    # Init
    def __init__(self, data):
        #self.routes = [[[] for x in range(n_vehicles) ] for y in range(n_depots)]
        self.chromosome = [[] for x in range(data.n_depots)]
        self.total_duration = [[0] for x in range(data.n_depots)]
        self.total_load = [[0] for x in range(data.n_depots)]
        self.route = [[] for depot in range(data.n_depots)]
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
            self.chromosome[depots].extend(list(customers_at_depot))

        # In case the amount of customers are not divisible by n_depots
        self.chromosome[-1].extend(customers)

    # Mutation functions
    def mutate(self, data):
        p = random()
        depot = randint(0, data.n_depots - 1)

        if p < 1/3.0:
            self._mutate_intra_swap(depot)

        elif p < 2/3.0:
            self._mutate_reversal(depot)

        else:
            self._mutate_single_re_routing(data)



            return 1
        return 0
    def mutate_inter_swap(self, data):
        depot1 = randint(0, data.n_depots - 1)
        depot2 = randint(0, data.n_depots - 1)

        if len(self.chromosome[depot1]) == 0 or len(self.chromosome[depot2]) == 0:
            return

        customer1 = randint(0, len(self.chromosome[depot1]) - 1)
        customer2 = randint(0, len(self.chromosome[depot2]) - 1)

        temp = self.chromosome[depot1][customer1]
        self.chromosome[depot1][customer1] = self.chromosome[depot2][customer2]
        self.chromosome[depot2][customer2] = temp
    def _mutate_intra_swap(self, depot):
        route = self.chromosome[depot]

        if len(route) == 0:
            return

        customer1 = randint(0, len(route) - 1)
        customer2 = randint(0, len(route) - 1)

        temp = route[customer1]
        route[customer1] = route[customer2]
        route[customer2] = temp
    def _mutate_reversal(self, depot):
        route = self.chromosome[depot]

        cutpoint_start = randint(0, len(route))
        cutpoint_stop = randint(cutpoint_start, len(route))

        # Reverse part of the chromosome
        if cutpoint_start == 0:
            route[cutpoint_start:cutpoint_stop] = route[cutpoint_stop - 1::-1]
        else:
            route[cutpoint_start:cutpoint_stop] = route[cutpoint_stop-1:cutpoint_start-1:-1]
    def _mutate_single_re_routing(self, data):
        depot = randint(0, data.n_depots-1)
        vehicle = randint(0, len(self.route[depot])-1)

        i = 0
        while len(self.route[depot][vehicle]) == 0:
            depot = randint(0, data.n_depots - 1)
            vehicle = randint(0, len(self.route[depot]) - 1)

            if i > 50:
                return
            i += 1

        i = randint(0, len(self.route[depot][vehicle])-1)
        customer = self.route[depot][vehicle][i]

        del self.route[depot][vehicle][i]
        self.insert_customer(depot, customer, True, data)
        self.route_to_chromosome(data)

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

    # Selection and recombination functions
    def schedule_routes(self, data):
        self.route = [[] for depot in range(data.n_depots)]
        self.total_duration = [[0] for x in range(data.n_depots)]
        self.total_load = [[0] for x in range(data.n_depots)]

        for depot in range(data.n_depots):
            customer_list = self.chromosome[depot] # Customers belonging to a given depot
            self.route[depot].append([])

            vehicle = 0
            for customer in customer_list:
                customer_duration = data.customer[customer].duration
                customer_load = data.customer[customer].load

                # Assign customers to a vehicle until the constraints are broken
                duration_limit_broken = self.total_duration[depot][vehicle] + customer_duration > data.depot[depot].duration_limit
                load_limit_broken = self.total_load[depot][vehicle] + customer_load > data.depot[depot].load_limit

                # Constraints broken. Assign customer to next vehicle
                if  duration_limit_broken or load_limit_broken:
                    vehicle += 1
                    self.route[depot].append([])
                    self.total_load[depot].append(0)
                    self.total_duration[depot].append(0)

                self.route[depot][vehicle].append(customer)
                self.total_duration[depot][vehicle] += customer_duration
                self.total_load[depot][vehicle] += customer_load
    def calculate_fitness(self, data):
        # Calculate total route distance for every vehicle
        total_distance = 0
        n_vehicles = 0

        self.schedule_routes(data)

        for depot in range(data.n_depots):
            for route in self.route[depot]:
                total_distance += self._calculate_route_distance(route, depot, data)
                n_vehicles += 1

        self.fitness = total_distance#100*n_vehicles + 0.01*total_distance

    def recombine(self, p2, data):
        # Choose a random depot for both parents
        depot = randint(0, data.n_depots - 1)

        # Choose a random route from each parent
        route1 = choice(self.route[depot])
        route2 = choice(p2.route[depot])

        c1 = deepcopy(self)
        c2 = deepcopy(p2)

        # Remove customer of the chosen crossover routes from the children
        c1.remove_customers(route2, data)
        c2.remove_customers(route1, data)

        # In this case, insert customer where cost is minimized regardless of feasibility
        k = random()
        maintain_feasibility = True
        if k >= 0.8:
            maintain_feasibility = False

        # Insert customers where route distance is minimized
        for customer in route2:
            c1.insert_customer(depot, customer, maintain_feasibility, data)
        for customer in route1:
            c2.insert_customer(depot, customer, maintain_feasibility, data)

        return c1, c2

    # Inserting and removing customers
    def calculate_insertion_costs(self, route, depot, customer, data):
        if len(route) == 0:
            return [data.distances_depot[depot][customer]*2]

        insertion_costs = []

        # Inserting at the front
        next_customer = route[0]
        added_distance = data.distances_depot[depot][customer] + data.distances[customer][next_customer]
        insertion_costs.append(added_distance)

        # Iterate through every possible insertion location
        for i in range(1, len(route)-1):
            next_customer = route[i + 1]
            prev_customer = route[i - 1]
            added_distance = data.distances[prev_customer][customer] + data.distances[customer][next_customer]

            insertion_costs.append(added_distance)

        # Inserting at the back
        prev_customer = route[len(route)-1]
        added_distance = data.distances[prev_customer][customer] + data.distances_depot[depot][customer]
        insertion_costs.append(added_distance)

        return insertion_costs
    def _limits_maintained(self, duration, load, depot, vehicle):
        duration_limit = data.depot[depot].duration_limit
        load_limit = data.depot[depot].load_limit

        current_duration = self.total_duration[depot][vehicle]
        current_load = self.total_load[depot][vehicle]

        duration_limit_maintained = current_duration + duration <= duration_limit
        load_limit_maintained = current_load + load <= load_limit

        return duration_limit_maintained and load_limit_maintained
    def insert_customer(self, depot, customer, maintain_feasibility, data):
        constraints_maintained = [None for i in range(len(self.route[depot]))]
        costs = [None for i in range(len(self.route[depot]))]
        customer_duration = data.customer[customer].duration
        customer_load = data.customer[customer].load

        # Check costs for inserting customer at every position in every route at depot
        # Check if constraints are maintained
        for vehicle in range(len(self.route[depot])):
            route = self.route[depot][vehicle]

            constraints_maintained[vehicle] = self._limits_maintained(customer_duration, customer_load, depot, vehicle)
            costs[vehicle] = self.calculate_insertion_costs(route, depot, customer, data)

        # Find the minimal insertion cost
        best_cost = 999999999
        insertion_index = 0
        route_index = 0
        for vehicle in range(len(costs)):
            for i in range(len(costs[vehicle])):
                current_cost = costs[vehicle][i]

                if  current_cost < best_cost and (constraints_maintained[vehicle] or not maintain_feasibility):
                    best_cost = current_cost
                    route_index = vehicle
                    insertion_index = i

        self.route[depot][route_index].insert(insertion_index, customer)
    def remove_customers(self, route, data):

        # Remove customers of the chosen crossover route
        for depot in range(data.n_depots):
            for vehicle in range(len(self.route[depot])):

                i = 0
                while i < len(self.route[depot][vehicle]):
                    customer = self.route[depot][vehicle][i]

                    if customer in route:
                        del self.route[depot][vehicle][i]
                        i -= 1

                    i += 1

    def route_to_chromosome(self, data):
        self.chromosome = [[] for i in range(data.n_depots)]
        for depot in range(data.n_depots):
            for vehicle in range(len(self.route[depot])):
                self.chromosome[depot].extend(self.route[depot][vehicle])


    # Operators
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
    population_size = 0

    # Genetic parameters
    mutation_rate = 0
    crossover_rate = 0

    # Selection parameters
    elitism_amount = 0
    rank_amount = 0
    tournament_amount = 0
    tournament_size = 0


    def __init__(self, data, population_size, mutation_rate, crossover_rate, elitism_amount, rank_amount, tournament_size):
        for individual in range(population_size):
            self.population.append(Phenotype(data))
            self.population[-1].permuted_init(data)

        self.mutation_rate = mutation_rate
        self.crossover_rate = crossover_rate
        self.elitism_amount = elitism_amount
        self.rank_amount = rank_amount
        self.tournament_size = tournament_size
        self.tournament_amount = 1.0 - rank_amount - elitism_amount
        self.population_size = population_size

    # Selection algorithms.
    def _rank_selection(self, sorted_population):
        parents = []

        # Rank selection
        weights = [j for j in range(len(sorted_population)) for i in range(j+1)]
        indexes = range(len(sorted_population))

        n_rank_individuals = int(self.population_size * self.rank_amount)
        while len(parents) < n_rank_individuals:
            individual = choice(weights)

            parents.append(sorted_population[individual])

            # Every individual can only be chosen once
            del_start = sum(indexes[:individual+1])
            del_stop = del_start+individual+1
            del weights[del_start:del_stop]
            del sorted_population[individual]
            indexes[individual] = 0

            for i in range(del_start, len(weights)):
                weights[i] -= 1

        return parents
    def _elitism_selection(self, sorted_population):
        n_elites = int(self.population_size * self.elitism_amount)
        return sorted_population[-n_elites:]
    def _tournament_selection(self):
        contestants = []
        parents = []
        n_tournament_individuals = int(self.population_size * (1 - (self.elitism_amount + self.rank_amount)))
        # Pick individuals
        for individual in range(n_tournament_individuals):

            # Pick a random set of contestants from the population
            start = randint(0, self.population_size - self.tournament_size)
            temp = self.population[start:start + self.tournament_size]

            # Sort contestants
            for k in range(self.tournament_size):
                insert_into(contestants, temp[k])

            p = random()
            # Pick best individual
            if p <= 0.8:
                i = -1

            # Pick random individual
            else:
                i = randint(0, self.tournament_size-1)

            parents.append(contestants[i])
        return parents
    def select_individuals(self):
        sorted_population = self.sort_individuals(data)

        # Select individuals
        parents_rank = self._rank_selection(sorted_population)
        elites = self._elitism_selection(sorted_population)
        parents_tournament = []#self._tournament_selection()

        print "Best individual:", elites[-1].fitness

        return parents_rank + parents_tournament + elites

    def sort_individuals(self, data):
        # Sort population by fitness
        sorted_population = []
        for individual in range(len(self.population)):
            self.population[individual].calculate_fitness(data)
            insert_into(sorted_population, self.population[individual])

        return sorted_population

    def create_next_generation(self, generation, parents, data):
        n_elites = int(self.population_size * self.elitism_amount)
        child_population = parents[-n_elites:]

        while len(child_population) < self.population_size:
            p1 = choice(parents)
            p2 = choice(parents)

            p = random()
            if p < self.crossover_rate:
                c1, c2 = p1.recombine(p2, data)

            else:
                c1 = p1
                c2 = p2

            # Translate the direct solution found from crossover to the indirect solution
            c1.route_to_chromosome(data)
            c2.route_to_chromosome(data)

            # Mutates the indirect solution
            p = random()
            if p <= self.mutation_rate:
                if generation % 10 != 0:
                    c1.mutate(data)
                    c2.mutate(data)
                else:
                    c1.mutate_inter_swap(data)
                    c2.mutate_inter_swap(data)

            child_population.extend([c1, c2])

        return child_population

    def step(self, generation, data):

        # Select through rank, tournament and elitism
        parents = self.select_individuals()

        # Run genetic operators
        self.population = self.create_next_generation(generation, parents, data)



## Util
def insert_into(list_a, element):
    if len(list_a) == 0:
        list_a.append(element)
        return

    for i in range(len(list_a)):
        if element > list_a[i]:
            list_a.insert(i, element)
            return
    list_a.append(element)

def plot_customers(data):
    i = 0
    x = []
    y = []
    while i < len(data.customer):
        x.append(data.customer[i].position.x)
        y.append(data.customer[i].position.y)
        i += 1

    plt.scatter(x, y, marker='+')

    # fig, ax = plt.subplots()
    #n = range(data.n_customers)
    #for i, txt in enumerate(n):
    #    ax.annotate(txt, (x[i], y[i]))
def plot_depots(data):
    i = 0
    x = []
    y = []
    while i < len(data.depot):
        x.append(data.depot[i].position.x)
        y.append(data.depot[i].position.y)
        i += 1
    plt.scatter(x, y, marker='o')
def plot_solution(solution, data):
    for depot in range(data.n_depots):
        for route in solution.route[depot]:
            x = []
            y = []
            x.append(data.depot[depot].position.x)
            y.append(data.depot[depot].position.y)

            for customer in route:
                x.append(data.customer[customer].position.x)
                y.append(data.customer[customer].position.y)

            x.append(data.depot[depot].position.x)
            y.append(data.depot[depot].position.y)
            plt.plot(x, y)
def plot_all(solution, data):
    plot_customers(data)
    plot_depots(data)
    plot_solution(solution, data)
    plt.show()


#####  MAIN  #####
# Read data
path = "Testing Data/Data Files/"
file_name = "p01"
data = Data(path + file_name)

## Run the genetic algorithm
n_generations = 500
population = 400
mutation_rate = 0.15
crossover_rate = 0.8
elitism_amount = 0.1
rank_amount = 0.9
tournament_size = 10

#http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.106.8662&rep=rep1&type=pdf

# Initialize population
run = GeneticAlgorithm(data, population, \
                       mutation_rate, crossover_rate, \
                       elitism_amount, rank_amount, tournament_size)

for generation in range(n_generations):
    print generation
    run.step(generation, data)

best_solution = run.population[6]
best_solution.calculate_fitness(data)
print "Final solution:", best_solution.fitness
plot_all(best_solution, data)
