#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <omp.h>

using namespace std;

const int POP_SIZE = 1000;
const int GENOME_SIZE = 20;
const int MAX_GENERATIONS = 100;
const double MUTATION_RATE = 0.01;

vector<bool> generateRandomGenome() {
    vector<bool> genome;
    for (int i = 0; i < GENOME_SIZE; ++i) {
        genome.push_back(rand() % 2);
    }
    return genome;
}

int calculateFitness(const vector<bool>& genome) {
    int fitness = 0;
    for (bool gene : genome) {
        if (gene) {
            fitness++;
        }
    }
    return fitness;
}

void mutate(vector<bool>& genome) {
    for (int i = 0; i < genome.size(); ++i) {
        if ((double) rand() / RAND_MAX < MUTATION_RATE) {
            genome[i] = !genome[i];
        }
    }
}

void geneticAlgorithm() {
    vector<vector<bool>> population(POP_SIZE, vector<bool>(GENOME_SIZE));
    #pragma omp parallel for
    for (int i = 0; i < POP_SIZE; ++i) {
        population[i] = generateRandomGenome();
    }

    vector<vector<bool>> offspring(POP_SIZE, vector<bool>(GENOME_SIZE));

    for (int generation = 1; generation <= MAX_GENERATIONS; ++generation) {
        vector<int> fitnesses(POP_SIZE);
        #pragma omp parallel for
        for (int i = 0; i < POP_SIZE; ++i) {
            fitnesses[i] = calculateFitness(population[i]);
        }

        #pragma omp parallel for
        for (int i = 0; i < POP_SIZE; ++i) {
            int idx1 = rand() % POP_SIZE;
            int idx2 = rand() % POP_SIZE;
            offspring[i] = (fitnesses[idx1] > fitnesses[idx2]) ? population[idx1] : population[idx2];
        }

        #pragma omp parallel for
        for (int i = 0; i < POP_SIZE; i += 2) {
            int crossoverPoint = rand() % GENOME_SIZE;
            for (int j = crossoverPoint; j < GENOME_SIZE; ++j) {
                swap(offspring[i][j], offspring[i + 1][j]);
            }
            mutate(offspring[i]);
            mutate(offspring[i + 1]);
        }
        #pragma omp parallel for
        for (int i = 0; i < POP_SIZE; ++i) {
            population[i] = offspring[i];
        }
    }
}

int main() {
    srand(time(0));
    clock_t startTime = clock();
    geneticAlgorithm();
    clock_t endTime = clock();

    double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;
    cout << "OpenMP execution time: " << elapsedTime << " seconds" << endl;

    return 0;
}
