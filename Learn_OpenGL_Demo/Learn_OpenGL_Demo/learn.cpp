//
//  learn.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/27/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include <stdio.h>
#include "learn.h"

const double SIM_TIME = 300;

Robot learn::learnNeuralNetwork(int generations, bool evolve){
    const int POPULATION_SIZE = 16;
    Robot startingRob = starting_models::getArrow();
    std::vector<glm::dvec3> springPositions;//these are the precious spring positions
    const glm::dvec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    
    //Initialize population
    double bestScore = 0.0;
    std::vector<std::tuple<NeuralNetwork,double> *> population;
    for (int i=0; i<POPULATION_SIZE; i++){
        NeuralNetwork nn(springPositions,neuronStartingPos, 3, 15);
        std::tuple<NeuralNetwork,double> *tup_pointer = new std::tuple<NeuralNetwork,double>(nn,bestScore);
        population.push_back(tup_pointer);
    }
    assert(population.size() == POPULATION_SIZE);
    
    #ifdef enable_graphics
    DotWriter dotWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
    VecWriter diversityWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
    #else
    DotWriter dotWriter("/home/lol2107");
    VecWriter diversityWriter("/home/lol2107");
    #endif
    //writer.appendData(bestScore,1);
    
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
            const double score = rob.simulate(0,SIM_TIME);
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
        
        for (auto pt : population)
            dotWriter.appendData((numGens-1)*(POPULATION_SIZE/2), std::get<1>(*pt));
        
        const int DIV_CHECK_WAIT = 5;
        if (numGens%DIV_CHECK_WAIT == 1){
            double averageDiversity = 0;
            for (auto pt1 : population){
                for (auto pt2 : population){
                    averageDiversity += (std::get<0>(*pt1).distanceFrom(std::get<0>(*pt2)));
                }
            }
            averageDiversity /= (POPULATION_SIZE * (POPULATION_SIZE-1))/2;
            diversityWriter.appendData(averageDiversity, (POPULATION_SIZE/2)*DIV_CHECK_WAIT);
        }
        
    }

    if (evolve){
        dotWriter.writeTo("evolveNeuralNetwork.csv");
        diversityWriter.writeTo("evolveNeuralNetworkDiv.csv");
    }
    else {
        dotWriter.writeTo("climbNeuralNetwork.csv");
        diversityWriter.writeTo("climbNeuralNetworkDiv.csv");
    }
    
    NeuralNetwork *bestNN = new NeuralNetwork(std::get<0>(*(population[0])));
    bestNN->writeTo("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/myNN.csv");
    assert(std::get<1>(*(population[0])) == bestScore);
    Robot rob = starting_models::getArrow();
    rob.setNN(bestNN);
    
//    Robot robcop1 = starting_models::getArrow();
//    robcop1.setNN(new NeuralNetwork(std::get<0>(*(population[0]))));
//    Robot robcop2 = starting_models::getArrow();
//    robcop2.setNN(new NeuralNetwork(std::get<0>(*(population[0]))));
//    float testSim1 = robcop1.simulate(0,60);
//    float testSim2 = robcop2.simulate(0,60);
//    ASSERT(testSim1 == testSim2, "testSim1 " << testSim1 << " testSim2 " << testSim2);
//    printf("PASSED!!!!!\n");
    PRINT_F(bestScore);
    
    return rob;
}








Robot learn::learnNeuralNetworkPareto(int generations){
    Robot startingRob = starting_models::getArrow();
    std::vector<glm::dvec3> springPositions;//these are the precious spring positions
    const glm::dvec3 neuronStartingPos = startingRob.calcCentroid();
    for (Spring *s : startingRob.getSprings())
        springPositions.push_back(s->calcCenter());
    
    
    double bestScore = 0.0;
    std::vector<std::tuple<NeuralNetwork, double, int> *> population;
    
    
#ifdef enable_graphics
    DotWriter dotWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
    VecWriter diversityWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
#else
    DotWriter dotWriter("/home/lol2107");
    VecWriter diversityWriter("/home/lol2107");
#endif
    
    int totalNumEvals = 0;
    
    const int NUMBER_OF_CORES = helper::numCores();
    assert(NUMBER_OF_CORES == 4);
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        //Update
        const long CUR_POPULATION_SIZE = population.size();
        
        std::cout << "Generation: " << numGens << "(" << CUR_POPULATION_SIZE << ") | " << bestScore << "\n";
        
        int numEvalsThisGen;
        if (CUR_POPULATION_SIZE >= 2)
            numEvalsThisGen = NUMBER_OF_CORES;
        else
            numEvalsThisGen = 1;
        
        assert(numEvalsThisGen == 1 || numEvalsThisGen == 4);
        assert(population.size() == CUR_POPULATION_SIZE);
        population.resize(CUR_POPULATION_SIZE+numEvalsThisGen, NULL);
        assert(population.size() == (CUR_POPULATION_SIZE+numEvalsThisGen));
        
        #pragma omp parallel for
        for (int k=NUMBER_OF_CORES-1; k>=0; k--){
            
            if (k == 0){
                //Add random
                NeuralNetwork nn(springPositions,neuronStartingPos, 3, 15);
                Robot rob = starting_models::getArrow();
                rob.setNN(&nn);
                const double score = rob.simulate(0,SIM_TIME);
                population[CUR_POPULATION_SIZE] = new std::tuple<NeuralNetwork,double,int>(nn, score, 0);
                //printf(" %f(1)",score);
            } else if (CUR_POPULATION_SIZE >= 2) {
                
                int index1 = helper::myrand(CUR_POPULATION_SIZE);
                int index2 = helper::myrand(CUR_POPULATION_SIZE,index1);
                NeuralNetwork offspring = std::get<0>(*(population[index1])).crossOver(std::get<0>(*(population[index2])));
                offspring.mutate();
                
                Robot rob = starting_models::getArrow();
                rob.setNN(&offspring);
                const double score = rob.simulate(0,SIM_TIME);
                
                const int age = std::max(std::get<2>(*(population[index1])), std::get<2>(*(population[index2])));
                
                population[CUR_POPULATION_SIZE+k] = new std::tuple<NeuralNetwork,double,int>(offspring,score,age);
                //printf(" %f(%d)",score,age);
            }
        }
        
        
        totalNumEvals += numEvalsThisGen;
        
        
        //Increment age
        for (auto pt : population)
            std::get<2>(*pt)++;
        
        //print population before cleaning
        printf("before: ");
        for (auto pt : population)
            printf(" (%g, %d)", std::get<1>(*pt),std::get<2>(*pt));
        
        //Remove dominated models
        for (long i=population.size()-1; i >= 0; i--){
            const double score = std::get<1>(*(population[i]));
            const int age = std::get<2>(*(population[i]));
            for (int j=0; j<population.size(); j++){
                const double cur_score = std::get<1>(*(population[j]));
                const int cur_age = std::get<2>(*(population[j]));
                if (i != j && score <= cur_score && age >= cur_age){
                    delete population[i];
                    population.erase(population.begin() + i);
                    break;
                }
            }
        }
        
        //print population after cleaning
        printf("\nafter: ");
        for (auto pt : population){
            printf(" (%g, %d)", std::get<1>(*pt),std::get<2>(*pt));
            dotWriter.appendData(totalNumEvals, std::get<1>(*pt));
        }
        
        //Write diversity
        double averageDiversity = 0;
        for (auto pt1 : population){
            for (auto pt2 : population){
                averageDiversity += (std::get<0>(*pt1).distanceFrom(std::get<0>(*pt2)));
            }
        }
        if (population.size()>1)
            averageDiversity /= (population.size() * (population.size()-1))/2;
        diversityWriter.appendData(averageDiversity, numEvalsThisGen);
        
        //Check for bestScore
        for (auto pt : population){
            if (std::get<1>(*pt) > bestScore){
                bestScore = std::get<1>(*pt);
                std::cout << "\nNew Record: " << bestScore << "(" << (std::get<2>(*pt)) << ")";
            }
        }
        printf("\n---------------------------\n");
        
        
    }
    
    dotWriter.writeTo("evolveNeuralNetworkPareto.csv");
    diversityWriter.writeTo("evolveNeuralNetworkParetoDiv.csv");
    
    //obv this is wrong
    NeuralNetwork *bestNN = new NeuralNetwork(std::get<0>(*(population[0])));
    bestNN->writeTo("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/myNN.csv");
    assert(std::get<1>(*(population[0])) == bestScore);
    Robot rob = starting_models::getArrow();
    rob.setNN(bestNN);
    
    return rob;
}







void learn::checkFeedback(NeuralNetwork &adam_nn, int numGens, bool evolve){
    
    Robot startingRob = starting_models::getArrow();
    
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
