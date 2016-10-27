#ifndef PARAMS_H_
#define PARAMS_H_

#include <random>
#include <cmath>
#include <iostream>
#include <fstream>

    const int input  = 2; //bias node and angle facing
    const int input_agents = 1; // number of agents to input into the NN


    const int output = 3;
    const int hiddenLayerSize = 0;
    const int hiddenLayers = 0;

    const int geneNN = ((input+input_agents*2)*output)*((hiddenLayerSize+1)*(hiddenLayers+1));
    const int geneCount = geneNN + input;
    
    const int generations = 100;
    const int timeTicks = 1000;

    const int replicates = 1;
    const int predator_pop_count = 1;
    const int prey_pop_count = 1;

    const double pred_capture = 8; 
    const double sensing_range_pred = 50;
    const double sensing_range_prey = 500;

    const double sizeX = 10000;
    const double sizeY = 10000;

    const double mut_mean = 0;
    const double mut_stddev = 1;
    const double mut_rate = 1/(double)geneCount;

    const double xover_rate = .5;

    const double pred_fitness = 2.0;

    const double prey_fitness = 100.0;

    extern std::fstream output_file_prey;
    extern std::fstream output_file_pred;

    extern std::random_device rd;
    extern std::mt19937 mutate;
    extern std::normal_distribution<> mutator;
    extern std::uniform_real_distribution<> rates;
    extern std::uniform_real_distribution<> rander;
    inline double normal_dist() {
        return mutator(mutate);
    }

    inline double get_mutation() {
        if (mut_rate < rates(mutate)) {
            return 0;
        }
        return mutator(mutate);
    }


#endif
