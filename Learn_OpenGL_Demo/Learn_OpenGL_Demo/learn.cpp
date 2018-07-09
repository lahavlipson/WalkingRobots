//
//  learn.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/27/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include <stdio.h>
#include "learn.h"

Robot learn::learnNeuralNetwork(int generations, bool evolve){
    const int POPULATION_SIZE = 64;
    Robot startingRob = starting_models::getArrow();
    std::vector<glm::dvec3> springPositions;//these are the precious spring positions
    const glm::dvec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    //Initialize population
    NeuralNetwork *nnp = new NeuralNetwork(springPositions,neuronStartingPos, 2, 12);
    assert(nnp->weights[0].rows() < 1000);
    startingRob.setNN(nnp);
    double bestScore = 0.0;
   // assert(bestScore != 0);
    std::vector<std::tuple<NeuralNetwork,double> *> population;
    for (int i=0; i<POPULATION_SIZE; i++){
        std::tuple<NeuralNetwork,double> *tup_pointer = new std::tuple<NeuralNetwork,double>(NeuralNetwork(*nnp),bestScore);
        population.push_back(tup_pointer);
    }
    assert(population.size() == POPULATION_SIZE);
    
    VecWriter writer("/Users/lahavlipson");
    writer.appendData(bestScore,1);
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        std::cout << "Generation: " << numGens << " | " << bestScore << "\nSims:";
        
        //Update
        #pragma omp parallel for
        for (int k=POPULATION_SIZE/2; k<POPULATION_SIZE; k++){

            int index1 = helper::myrand(POPULATION_SIZE/2);
            int index2 = helper::myrand(POPULATION_SIZE/2,index1);
            assert(index1 != index2);
            //PRINT_I(index1);PRINT_I(index2);
            
            NeuralNetwork offspring;
            if (evolve){
                offspring = std::get<0>(*(population[index1])).crossOver(std::get<0>(*(population[index2])));
            } else {
                offspring = std::get<0>(*(population[index1]));
            }
            //offspring.calculateNeuronPositions();
            offspring.mutate();
            //checkFeedback(offspring,40,false);
            
            Robot rob = starting_models::getArrow();
            rob.setNN(&offspring);
            const double score = rob.simulate(0,60);
           // assert(score != 0);
            assert(k != 0);
            delete population[k];
            population[k] = new std::tuple<NeuralNetwork,double>(offspring,score);
            //std::cout << "Sim: " << score << std::endl;
            printf(" %f",score);
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
            std::tuple<NeuralNetwork,double> *tmp = population[i];
            population[i] = population[max_idx];
            population[max_idx] = tmp;
        }

        
        if (std::get<1>(*(population[0])) > bestScore){
            bestScore = std::get<1>(*(population[0]));
            std::cout << "\nNew Record: " << bestScore;
        }
        printf("\n---------------------------\n");
        
        writer.appendData(bestScore,POPULATION_SIZE/2);
        
    }

    writer.writeTo("Personal\ Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs/evolveNeuralNetwork.csv");
    
    NeuralNetwork *bestNN = new NeuralNetwork(std::get<0>(*(population[0])));
    assert(std::get<1>(*(population[0])) == bestScore);
    Robot rob = starting_models::getArrow();
    rob.setNN(bestNN);
    
    Robot robcop1 = starting_models::getArrow();
    robcop1.setNN(new NeuralNetwork(std::get<0>(*(population[0]))));
    Robot robcop2 = starting_models::getArrow();
    robcop2.setNN(new NeuralNetwork(std::get<0>(*(population[0]))));
    float testSim1 = robcop1.simulate(0,60);
    float testSim2 = robcop2.simulate(0,60);
    ASSERT(testSim1 == testSim2, "testSim1 " << testSim1 << " testSim2 " << testSim2);
    printf("PASSED!!!!!\n");
    
    return rob;
}











/*

//TODO RUN AND TEST THIS
Robot learn::climbNeuralNetwork(int generations){
    std::array<std::tuple<NeuralNetwork,double> *,MAX_SIZE_NEEDED> mutations;
    for (auto &ptr : mutations) ptr = NULL;
    
    //Initialize
    Robot startingRob = starting_models::getArrow();
    std::vector<glm::dvec3> springPositions;//these are the precious spring positions
    const glm::dvec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    
    NeuralNetwork bestNN(springPositions,neuronStartingPos, 7, 15);
    startingRob.setNN(&bestNN);
    double bestScore = startingRob.simulate(0,30);
    
    //writer
    VecWriter writer("/Users/lahavlipson");
    writer.appendData(bestScore,1);
    int numCores;
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        std::cout << "Generation: " << numGens << " | " << bestScore << std::endl;
        
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
                
                Robot rob = starting_models::getArrow();
                rob.setNN(&mutatedNN);
                double score = rob.simulate(0,40);
                mutations[i] = new std::tuple<NeuralNetwork,double>(mutatedNN,score);
                std::cout << "Sim: " << score << std::endl;
            }
        }
        
        for (auto &tup : mutations){
            if (tup != NULL){
                double score = std::get<1>(*tup);
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
    
    Robot rob = starting_models::getArrow();
    rob.setNN(new NeuralNetwork(bestNN));
    return rob;
}

*/









void learn::checkFeedback(NeuralNetwork &adam_nn, int numGens, bool evolve){
    
    Robot startingRob = starting_models::getArrow();
    const glm::dvec3 neuronStartingPos = startingRob.calcCentroid();
    
    const std::vector<Spring *> vecToCopy = startingRob.orderedListOfSprings;
    std::vector<glm::dvec3> springPositions;
    for (int i=0; i<vecToCopy.size(); i++)
        springPositions.push_back(vecToCopy[i]->calcCenter());
    
    double bestScore = 0.0;
    
    const int POPULATION_SIZE = 1;
    
    std::array<std::tuple<NeuralNetwork,double> *,POPULATION_SIZE> population;
    for (auto &e : population){
       // NeuralNetwork adam_nn(springPositions,neuronStartingPos, 7, 13);
        e = new std::tuple<NeuralNetwork,double>(adam_nn, bestScore);
    }
    
    VecWriter writer("/Users/lahavlipson");
    writer.appendData(bestScore,1);
    
    for (int gen=1;gen<=numGens;gen++){
        
       // std::cout << "NN Generation: "  << gen << " | " << bestScore << "\n";
        
        //#pragma omp parallel for
        for (int n=POPULATION_SIZE/2; n<POPULATION_SIZE; n++){
            
            //Create springs for simulation
            std::vector<Spring *> dummysprings;
            for (int i=0;i<vecToCopy.size();i++)
                dummysprings.push_back(new DummySpring(*vecToCopy[i]));
            
            //create testNN
            NeuralNetwork test_nn;
            const int index1 = helper::myrand(POPULATION_SIZE/2);
            const int index2 = helper::myrand(POPULATION_SIZE/2, index1);
            if (evolve){
                assert(POPULATION_SIZE>1);
                test_nn = std::get<0>(*population[index1]).crossOver(std::get<0>(*population[index2]));
            }
            else {
                test_nn = std::get<0>(*population[index1]);
            }
            
            test_nn.calculateNeuronPositions();
            for (int i=1+int(rand()%4);i>0;i--)
                test_nn.mutate();
            const NeuralNetwork constTest_nn(test_nn);
            
            //Simulate
            std::vector<std::vector<double>> timeSerieses(dummysprings.size());
            const unsigned int startRec = 200;
            const unsigned int endRec = 700;
            for (int i=0; i<endRec; i++){
                test_nn.evaluate(dummysprings);
                if (i >= startRec){
                    for (int j=0; j<dummysprings.size(); j++)
                        timeSerieses[j].push_back(dummysprings[j]->calcLength());
                }
            }
            
            //Score
            double totalVariance = 0.0;
            for (int i=0; i<dummysprings.size(); i++){
                assert(timeSerieses[i].size() == (endRec-startRec));
                totalVariance += helper::calcVariance(timeSerieses[i]);
                //std::cout << dummysprings[i]->calcLength() << " ";
            }
            //    std::cout << " tv:" << totalVariance << "\n\n";

            
            //Add to population
            //std::cout << "Sim: " << totalVariance << std::endl;
            delete population[n];
            population[n] = new std::tuple<NeuralNetwork, double>(constTest_nn,totalVariance);
        }
        
        
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
            std::tuple<NeuralNetwork,double> *tmp = population[i];
            population[i] = population[max_idx];
            population[max_idx] = tmp;
        }
        
        
        //Update best score (Just for keeping track)
        if (std::get<1>(*(population[0])) > bestScore){
            bestScore = std::get<1>(*(population[0]));
            //std::cout << "New Record: " << bestScore << std::endl;
        }
        
        writer.appendData(bestScore,POPULATION_SIZE/2);
    }
    
    writer.writeTo("feedback_nn.csv");
    
    adam_nn = std::get<0>(*(population[0]));
//    startingRob.setNN(new NeuralNetwork(std::get<0>(*(population[0]))));
//    return startingRob;
    
}
