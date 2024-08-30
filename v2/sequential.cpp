#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// Define constants
const int POP_SIZE = 1000;
const int GENOME_SIZE = 20;
const int MAX_GENERATIONS = 100;
const double MUTATION_RATE = 0.01;

// Function to generate a random genome
vector<bool> generateRandomGenome() {
    vector<bool> genome;
    for (int i = 0; i < GENOME_SIZE; ++i) {
        genome.push_back(rand() % 2);
    }
    return genome;
}

// Function to calculate fitness of a genome
int calculateFitness(const vector<bool>& genome) {
    int fitness = 0;
    for (bool gene : genome) {
        if (gene) {
            fitness++;
        }
    }
    return fitness;
}

// Function to perform mutation
void mutate(vector<bool>& genome) {
    for (int i = 0; i < genome.size(); ++i) {
        if ((double) rand() / RAND_MAX < MUTATION_RATE) {
            genome[i] = !genome[i];
        }
    }
}

// Main genetic algorithm function
void geneticAlgorithm() {
    // Initialize population
    vector<vector<bool>> population;
    for (int i = 0; i < POP_SIZE; ++i) {
        population.push_back(generateRandomGenome());
    }

    // Main loop
    for (int generation = 1; generation <= MAX_GENERATIONS; ++generation) {
        // Evaluate fitness of each individual
        vector<int> fitnesses;
        for (const auto& genome : population) {
            fitnesses.push_back(calculateFitness(genome));
        }

        // Select parents based on fitness (tournament selection)
        vector<vector<bool>> parents;
        for (int i = 0; i < POP_SIZE; ++i) {
            int idx1 = rand() % POP_SIZE;
            int idx2 = rand() % POP_SIZE;
            parents.push_back(fitnesses[idx1] > fitnesses[idx2] ? population[idx1] : population[idx2]);
        }

        // Crossover and mutation
        for (int i = 0; i < POP_SIZE; i += 2) {
            int crossoverPoint = rand() % GENOME_SIZE;
            for (int j = crossoverPoint; j < GENOME_SIZE; ++j) {
                swap(parents[i][j], parents[i + 1][j]);
            }
            mutate(parents[i]);
            mutate(parents[i + 1]);
        }

        // Replace population with offspring
        population = parents;
    }
}

int main() {
    srand(time(0)); // Seed the random number generator
    clock_t startTime = clock(); // Start time measurement
    geneticAlgorithm();
    clock_t endTime = clock(); // End time measurement

    double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;
    cout << "Sequential execution time: " << elapsedTime << " seconds" << endl;

    return 0;
}
