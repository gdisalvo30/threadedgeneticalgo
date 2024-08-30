// C++ program to create target string, starting from 
// random string using Genetic Algorithm 

#include <bits/stdc++.h> 
using namespace std; 
#include <time.h>
#include <omp.h>

// Number of individuals in each generation 
#define POPULATION_SIZE 100 

// Valid Genes 
const string GENES = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOP"\ 
"QRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}"; 

// Target string to be generated 
const string TARGET = "buzz buzz buzz buzz buzz buzz buzz buzz buzz buzz buzz"; 

// Function to generate random numbers in given range 
int random_num(int start, int end) 
{ 
	int range = (end-start)+1; 
	int random_int = start+(rand()%range); 
	return random_int; 
} 

// Create random genes for mutation 
char mutated_genes() 
{ 
	int len = GENES.size(); 
	int r = random_num(0, len-1); 
	return GENES[r]; 
} 

// create chromosome or string of genes 
string create_gnome() 
{ 
	int len = TARGET.size(); 
	string gnome = ""; 
	for(int i = 0;i<len;i++) 
		gnome += mutated_genes(); 
	return gnome; 
} 

// Class representing individual in population 
class Individual 
{ 
public: 
	string chromosome; 
	int fitness; 
	Individual(string chromosome); 
	Individual mate(Individual parent2); 
	int cal_fitness(); 
}; 

Individual::Individual(string chromosome) 
{ 
	this->chromosome = chromosome; 
	fitness = cal_fitness(); 
}; 

// Perform mating and produce new offspring 
Individual Individual::mate(Individual par2) 
{ 
	// chromosome for offspring 
	string child_chromosome = ""; 

	int len = chromosome.size();
	for(int i = 0;i<len;i++) 
	{ 
		// random probability 
		float p = random_num(0, 100)/100; 

		// if prob is less than 0.45, insert gene 
		// from parent 1 
		if(p < 0.45) 
			child_chromosome += chromosome[i]; 

		// if prob is between 0.45 and 0.90, insert 
		// gene from parent 2 
		else if(p < 0.90) 
			child_chromosome += par2.chromosome[i]; 

		// otherwise insert random gene(mutate), 
		// for maintaining diversity 
		else
			child_chromosome += mutated_genes(); 
	} 

	// create new Individual(offspring) using 
	// generated chromosome for offspring 
	return Individual(child_chromosome); 
}; 


// Calculate fitness score, it is the number of 
// characters in string which differ from target 
// string. 
int Individual::cal_fitness() 
{ 
	int len = TARGET.size(); 
	int fitness = 0; 
	for(int i = 0;i<len;i++) 
	{ 
		if(chromosome[i] != TARGET[i]) 
			fitness++; 
	} 
	return fitness;	 
}; 

// Overloading < operator 
bool operator<(const Individual &ind1, const Individual &ind2) 
{ 
	return ind1.fitness < ind2.fitness; 
} 

// Driver code 
int main(int argc, char *argv[]) {
    int num_threads = atoi(argv[1]); // Number of threads specified by command line argument
    omp_set_num_threads(num_threads); // Set number of OpenMP threads

    timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); 

    for (int repeat = 0; repeat < 8; ++repeat) {
        srand((unsigned)(time(0))); 
        int generation = 0; 
        std::vector<Individual> population; 
        bool found = false; 

        #pragma omp parallel // Start parallel region
        {
            while (!found) {
                #pragma omp single // Ensure that only one thread executes the following block
                {
                    // Create initial population
                    population.clear(); // Clear population vector to prevent race condition
                    for(int i = 0;i<POPULATION_SIZE;i++) {
                        std::string gnome = create_gnome(); 
                        population.push_back(Individual(gnome)); 
                    }
                }

                // Sort the population in increasing order of fitness score
                #pragma omp single
                {
                    sort(population.begin(), population.end()); 
                    if(population[0].fitness <= 0) {
                        std::cout<< "Generation: " << generation << "\t"; 
                        std::cout<< "Fitness: "<< population[0].fitness << "\n"; 
                        std::cout<< "String: "<< population[0].chromosome <<"\t"; 
                        found = true; 
                        break; 
                    }
                }

                // Generate new offsprings for new generation
                std::vector<Individual> new_generation; 

                // Perform Elitism
                int s = (10*POPULATION_SIZE)/100; 
                #pragma omp single
                {
                    for(int i = 0;i<s;i++) {
                        new_generation.push_back(population[i]); 
                    }
                }

                // Mate individuals to produce offspring
                s = (90*POPULATION_SIZE)/100; 
                #pragma omp for
                for(int i = 0;i<s;i++) { 
                    int len = population.size(); 
                    int r = random_num(0, 50); 
                    Individual parent1 = population[r]; 
                    r = random_num(0, 50); 
                    Individual parent2 = population[r]; 
                    Individual offspring = parent1.mate(parent2); 
                    new_generation.push_back(offspring); 
                } 

                // Replace population with new generation
                #pragma omp single
                {
                    population = new_generation; 
                    generation++; 
                }
            }
        } // End of parallel region
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long seconds = end_time.tv_sec - start_time.tv_sec;
    long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
    long elapsed_ms = (seconds * 1000) + (nanoseconds / 1000000);
    std::cout << "Time elapsed for 10 iterations: " << elapsed_ms << " ms" << std::endl;
}