//
//  learn.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/27/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include <stdio.h>
#include "learn.h"

Robot learn::evolveNeuralNetwork(int generations){
    const int POPULATION_SIZE = 88;
    Robot startingRob = starting_models::getTetroid();
    std::vector<glm::vec3> springPositions;//these are the precious spring positions
    const glm::vec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    //Initialize population
    NeuralNetwork *nnp = new NeuralNetwork(springPositions,neuronStartingPos, 4, 10);
    assert(nnp->weights[0].rows() < 1000);
    startingRob.setNN(nnp);
    float bestScore = startingRob.simulate(0,10);
    assert(bestScore != 0);
    std::vector<std::tuple<NeuralNetwork,float> *> population;
    for (int i=0; i<POPULATION_SIZE; i++){
        std::tuple<NeuralNetwork,float> *tup_pointer = new std::tuple<NeuralNetwork,float>(NeuralNetwork(*nnp),bestScore);
        population.push_back(tup_pointer);
    }
    assert(population.size() == POPULATION_SIZE);
    
    
    //loop
    for (int numGens = 0; numGens < generations; numGens++){
        
        PRINT(numGens);
        
        assert(population.size() == POPULATION_SIZE);
        //Rank
        int i, j, max_idx;
        for (i = 0; i < POPULATION_SIZE-1; i++)
        {
            // Find the maximum element in unsorted array
            max_idx = i;
            for (j = i+1; j < POPULATION_SIZE; j++){
                if (std::get<1>(*(population[j])) > std::get<1>(*(population[max_idx])))
                    max_idx = j;
            }
            
            // Swap the found maximum element with the first element (actually swapping their pointers)
            std::tuple<NeuralNetwork,float> *tmp = population[i];
            population[i] = population[max_idx];
            population[max_idx] = tmp;
        }
        
        //Update
        #pragma omp parallel for
        for (int k=POPULATION_SIZE/2; k<POPULATION_SIZE; k++){

            int index1 = helper::myrand(POPULATION_SIZE/2);
            int index2 = helper::myrand(POPULATION_SIZE/2,index1);
            assert(index1 != index2);
            //PRINT_I(index1);PRINT_I(index2);
            
            NeuralNetwork parent1 = std::get<0>(*(population[index1]));
            NeuralNetwork parent2 = std::get<0>(*(population[index2]));
            NeuralNetwork offspring = parent1.crossOver(parent2);
            offspring.calculateNeuronPositions();
            offspring.mutate(3.0f);
            
            Robot rob = starting_models::getTetroid();
            rob.setNN(&offspring);
            const float score = rob.simulate(0,10);
            assert(score != 0);
            assert(k != 0);
            delete population[k];
            population[k] = new std::tuple<NeuralNetwork,float>(offspring,score);
            std::cout << "sim!: " << score << std::endl;
        }
        
        if (std::get<1>(*(population[1])) > bestScore){
            bestScore = std::get<1>(*(population[0]));
            std::cout << "New Record: " << bestScore << std::endl;
        }
        
    }

    
    return starting_models::getTetrahedron();
}
