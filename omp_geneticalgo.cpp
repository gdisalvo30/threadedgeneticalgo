// C++ program to create target string, starting from 
// random string using Genetic Algorithm 

#include <bits/stdc++.h> 
using namespace std; 
#include <omp.h>
#include <time.h>

// Number of individuals in each generation 
#define POPULATION_SIZE 100 

// Valid Genes 
const string GENES = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOP" 
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
    vector<string> partial_chromosomes(omp_get_max_threads(), ""); // Vector to hold partial chromosomes

    #pragma omp parallel for
    for(int i = 0; i < len; i++) 
    { 
        // random probability 
        float p = random_num(0, 100) / 100; 

        // Determine which parent to take gene from
        char gene;
        if(p < 0.45) 
            gene = chromosome[i]; 
        else if(p < 0.90) 
            gene = par2.chromosome[i]; 
        else
            gene = mutated_genes(); 

        // Append gene to partial chromosome
        int thread_num = omp_get_thread_num();
        partial_chromosomes[thread_num] += gene;
    } 

    // Combine partial chromosomes from all threads
    for (const auto &partial : partial_chromosomes) {
        child_chromosome += partial;
    }

    // create new Individual(offspring) using 
    // generated chromosome for offspring 
    return Individual(child_chromosome); 
}




// Calculate fitness score, it is the number of 
// characters in string which differ from target 
// string. 
int Individual::cal_fitness() 
{ 
    int len = TARGET.size();
    int fitness = 0;

    #pragma omp parallel for reduction(+:fitness)
    for (int i = 0; i < len; i++) {
        if (chromosome[i] != TARGET[i])
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
int main(int argc, char *argv[]) // Accept num_threads as command line argument
{ 
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <num_threads>" << endl;
        return 1;
    }

    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads); // Set the number of threads
    timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); 

    for (int repeat = 0; repeat < 10; ++repeat) {
        srand((unsigned)(time(0))); 
        // current generation 
        int generation = 0; 

        vector<Individual> population; 
        bool found = false; 

        // create initial population 
        for(int i = 0;i<POPULATION_SIZE;i++) 
        { 
            string gnome = create_gnome(); 
            population.push_back(Individual(gnome)); 
        }

        while(!found) 
        { 
            // sort the population in increasing order of fitness score 
            sort(population.begin(), population.end()); 

            // if the individual having lowest fitness score ie. 
            // 0 then we know that we have reached to the target 
            // and break the loop 
            if(population[0].fitness <= 0) 
            { 
                cout<< "Generation: " << generation << "\t"; 
                cout<< "Fitness: "<< population[0].fitness << "\n"; 
                cout<< "String: "<< population[0].chromosome <<"\t"; 
                found = true; 
                break; 
            } 

            // Otherwise generate new offsprings for new generation 
            vector<Individual> new_generation; 

            // Perform Elitism, that mean 10% of fittest population 
            // goes to the next generation 
            int s = (10*POPULATION_SIZE)/100; 
            for(int i = 0;i<s;i++) 
            {
                new_generation.push_back(population[i]); 
            }

            // From 50% of fittest population, Individuals 
            // will mate to produce offspring 
            s = (90*POPULATION_SIZE)/100; 
            for(int i = 0;i<s;i++) 
            { 
                int r = random_num(0, 50); 
                Individual parent1 = population[r]; 
                r = random_num(0, 50); 
                Individual parent2 = population[r]; 
                Individual offspring = parent1.mate(parent2); 
                new_generation.push_back(offspring); 
            } 

            population = new_generation; 

            generation++;  
        } 
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long seconds = end_time.tv_sec - start_time.tv_sec;
    long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
    long elapsed_ms = (seconds * 1000) + (nanoseconds / 1000000);
    cout << "Time elapsed for 10 iterations: " << elapsed_ms << " ms" << endl;
}
