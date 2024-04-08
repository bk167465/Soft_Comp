#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using namespace std;

// Parameters
int POP_SIZE;
int MAX_GEN;
double CROSSOVER_RATE;
double MUTATION_RATE;

// Objective functions
double f1(double x, double y) {
    return x * x - y;
}

double f2(double x, double y) {
    return -0.5 * x - y - 1;
}

// Constraint functions
double g1(double x, double y) {
    return 6.5 - x / 6 - y;
}

double g2(double x, double y) {
    return 7.5 - 0.5 * x - y;
}

double g3(double x, double y) {
    return 30 - 5 * x - y;
}

// Individual representation
struct Individual {
    double x, y;
    double fitness1, fitness2;
    bool dominated;
};

// Generate a random number between min and max
double rand_double(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

// Initialize the population with random individuals
vector<Individual> initialize_population() {
    vector<Individual> population;
    for (int i = 0; i < POP_SIZE; ++i) {
        Individual ind;
        ind.x = rand_double(-7, 4);
        ind.y = rand_double(-7, 4);
        ind.fitness1 = f1(ind.x, ind.y);
        ind.fitness2 = f2(ind.x, ind.y);
        ind.dominated = false;
        population.push_back(ind);
    }
    return population;
}

// Determine if individual1 dominates individual2
bool dominates(const Individual &individual1, const Individual &individual2) {
    return (individual1.fitness1 <= individual2.fitness1 && individual1.fitness2 <= individual2.fitness2 &&
            (individual1.fitness1 < individual2.fitness1 || individual1.fitness2 < individual2.fitness2));
}

// Assign crowding distance to individuals in the front
void assign_crowding_distance(vector<Individual> &front) {
    int size = front.size();
    for (auto &ind : front) {
        ind.dominated = false;
    }

    sort(front.begin(), front.end(), [](const Individual &a, const Individual &b) {
        return a.fitness1 < b.fitness1;
    });
    front.front().dominated = front.back().dominated = true;

    sort(front.begin(), front.end(), [](const Individual &a, const Individual &b) {
        return a.fitness2 < b.fitness2;
    });
    front.front().dominated = front.back().dominated = true;

    for (int i = 1; i < size - 1; ++i) {
        front[i].dominated = true;
    }
}

// Perform crossover between parents to produce offspring
Individual crossover(const Individual &parent1, const Individual &parent2) {
    Individual offspring;
    if (rand_double(0, 1) < CROSSOVER_RATE) {
        offspring.x = (parent1.x + parent2.x) / 2;
        offspring.y = (parent1.y + parent2.y) / 2;
    } else {
        offspring.x = parent1.x;
        offspring.y = parent2.y;
    }
    return offspring;
}

// Perform mutation on the offspring
void mutate(Individual &offspring) {
    if (rand_double(0, 1) < MUTATION_RATE) {
        double mutation_factor = rand_double(0.8, 1.2);
        offspring.x *= mutation_factor;
        offspring.y *= mutation_factor;
    }
}

// NSGA-II main algorithm
void nsga2() {
    // Initialize population
    vector<Individual> population = initialize_population();

    for (int gen = 0; gen < MAX_GEN; ++gen) {
        // Create empty next generation
        vector<Individual> next_generation;

        // Tournament selection
        for (int i = 0; i < POP_SIZE; ++i) {
            // Select two individuals randomly
            int idx1 = rand() % POP_SIZE;
            int idx2 = rand() % POP_SIZE;
            while (idx2 == idx1) {
                idx2 = rand() % POP_SIZE;
            }

            // Choose the better individual
            const Individual &parent1 = population[idx1];
            const Individual &parent2 = population[idx2];
            Individual offspring = dominates(parent1, parent2) ? parent1 : parent2;

            // Perform crossover and mutation
            offspring = crossover(parent1, parent2);
            mutate(offspring);

            next_generation.push_back(offspring);
        }

        // Merge current and next generation
        population.insert(population.end(), next_generation.begin(), next_generation.end());

        // Non-dominated sorting
        vector<Individual> front;
        for (auto &ind : population) {
            if (!ind.dominated) {
                front.push_back(ind);
            }
        }
        assign_crowding_distance(front);

        // Select individuals for next generation
        population.clear();
        int i = 0;
        while (population.size() + front.size() <= POP_SIZE) {
            for (auto &ind : front) {
                population.push_back(ind);
            }
            ++i;
        }

        sort(front.begin(), front.end(), [](const Individual &a, const Individual &b) {
            return (a.fitness1 + a.fitness2) > (b.fitness1 + b.fitness2);
        });

        for (int j = 0; j < POP_SIZE - population.size(); ++j) {
            population.push_back(front[j]);
        }
    }

    // Print the final population
    for (size_t i = 0; i < POP_SIZE; ++i) {
        cout << "Solution " << i + 1 << ": ";
        cout << "x=" << population[i].x << ", y=" << population[i].y;
        cout << " Fitness: " << population[i].fitness1 << ", " << population[i].fitness2 << '\n';
    }
}

int main() {
    // Seed random number generator
    srand(time(nullptr));

    cout<<"Enter the Population Size: ";
    cin>>POP_SIZE;

    cout<<"\nEnter the number of Generations: ";
    cin>>MAX_GEN;

    cout<<"\nEnter the Crossover Rate: ";
    cin>>CROSSOVER_RATE;

    cout<<"\nEnter the Mutation Rate: ";
    cin>>MUTATION_RATE;

    // Run NSGA-II
    nsga2();

    return 0;
}

