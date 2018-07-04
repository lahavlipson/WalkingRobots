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
    const int POPULATION_SIZE = 96;
    Robot startingRob = starting_models::getArrow();
    std::vector<glm::vec3> springPositions;//these are the precious spring positions
    const glm::vec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    //Initialize population
    NeuralNetwork *nnp = new NeuralNetwork(springPositions,neuronStartingPos, 4, 10);
    assert(nnp->weights[0].rows() < 1000);
    startingRob.setNN(nnp);
    float bestScore = startingRob.simulate(0,30);
   // assert(bestScore != 0);
    std::vector<std::tuple<NeuralNetwork,float> *> population;
    for (int i=0; i<POPULATION_SIZE; i++){
        std::tuple<NeuralNetwork,float> *tup_pointer = new std::tuple<NeuralNetwork,float>(NeuralNetwork(*nnp),bestScore);
        population.push_back(tup_pointer);
    }
    assert(population.size() == POPULATION_SIZE);
    
    VecWriter writer("/Users/lahavlipson");
    writer.appendData(bestScore,1);
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        std::cout << "Gen: " << numGens << " | " << bestScore << std::endl;
        
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
            offspring.mutate();
            
            Robot rob = starting_models::getArrow();
            rob.setNN(&offspring);
            const float score = rob.simulate(0,30);
           // assert(score != 0);
            assert(k != 0);
            delete population[k];
            population[k] = new std::tuple<NeuralNetwork,float>(offspring,score);
            std::cout << "Sim: " << score << std::endl;
        }
        
        
        
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
        
        
        
        
        if (std::get<1>(*(population[1])) > bestScore){
            bestScore = std::get<1>(*(population[0]));
            std::cout << "New Record: " << bestScore << std::endl;
        }
        
        writer.appendData(bestScore,POPULATION_SIZE/2);
        
    }

    writer.writeTo("Personal\ Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs/evolveNeuralNetwork.csv");
    
    NeuralNetwork *bestNN = new NeuralNetwork(std::get<0>(*(population[0])));
    Robot rob = starting_models::getArrow();
    rob.setNN(bestNN);
    return rob;
}

//TODO RUN AND TEST THIS
Robot learn::climbNeuralNetwork(int generations){
    const int MAX_SIZE_NEEDED = 96;
    std::array<std::tuple<NeuralNetwork,float> *,MAX_SIZE_NEEDED> mutations;
    for (auto &ptr : mutations) ptr = NULL;
    
    //Initialize
    Robot startingRob = starting_models::getTetroid();
    std::vector<glm::vec3> springPositions;//these are the precious spring positions
    const glm::vec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    
    NeuralNetwork bestNN(springPositions,neuronStartingPos, 4, 10);
    startingRob.setNN(&bestNN);
    float bestScore = startingRob.simulate(0,30);
    
    //writer
    VecWriter writer("/Users/lahavlipson");
    writer.appendData(bestScore,1);
    int numCores;
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        std::cout << "Gen: " << numGens << " | " << bestScore << std::endl;
        
        #pragma omp parallel
        {
            numCores = omp_get_num_threads();
            assert(numCores > 1);
            
            #pragma omp for
            for (int i=0;i<numCores;i++){
                assert(mutations[i] == NULL);
                
                NeuralNetwork mutatedNN = bestNN;
                mutatedNN.calculateNeuronPositions();
                mutatedNN.mutate();
                
                Robot rob = starting_models::getTetroid();
                rob.setNN(&mutatedNN);
                float score = rob.simulate(0,20);
                mutations[i] = new std::tuple<NeuralNetwork,float>(mutatedNN,score);
                std::cout << "Sim: " << score << std::endl;
            }
        }
        
        for (auto &tup : mutations){
            if (tup != NULL){
                float score = std::get<1>(*tup);
                if (score > bestScore){
                    bestScore = score;
                    bestNN = std::get<0>(*tup);
                    std::cout << "New Record: " << bestScore << std::endl;
                }
                tup = NULL;
            }
        }
        
        writer.appendData(bestScore,numCores);
    }
    
    
    writer.writeTo("Personal\ Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs/climbNeuralNetwork.csv");
    
    Robot rob = starting_models::getTetroid();
    rob.setNN(new NeuralNetwork(bestNN));
    return rob;
}
