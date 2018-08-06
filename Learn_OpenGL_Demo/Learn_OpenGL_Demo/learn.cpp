//
//  learn.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/27/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include <stdio.h>
#include "learn.h"
#include <float.h>


Robot learn::learnNeuralNetworkPareto(int generations){
    
    double bestScore = 0.0;
    std::vector<Individual> population;
    const int MAX_POP_SIZE = std::max(helper::numCores()*2-2, 14);
    
#ifdef enable_graphics
    DotWriter dotWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
    VecWriter diversityWriter("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/Learn_OpenGL_Demo/graphs");
#else
    DotWriter dotWriter("/home/lol2107");
    VecWriter diversityWriter("/home/lol2107");
#endif
    
    int totalNumEvals = 0;
    
    //loop
    for (int numGens = 1; numGens <= generations; numGens++){
        
        //Update
        const long CUR_POPULATION_SIZE = population.size();
        
        std::cout << "Generation: " << numGens << "(" << CUR_POPULATION_SIZE << ") | " << bestScore << "\n";
        
        //shuffle pop
        std::random_shuffle(population.begin(),population.end());
        
        long numEvalsThisGen;
        if (CUR_POPULATION_SIZE == 0)
            numEvalsThisGen = 1;
        else if (CUR_POPULATION_SIZE < 10)
            numEvalsThisGen = 2 + CUR_POPULATION_SIZE/2;
        else
            numEvalsThisGen = 1 + CUR_POPULATION_SIZE/2;
        
        assert(population.size() == CUR_POPULATION_SIZE);
        population.resize(CUR_POPULATION_SIZE+numEvalsThisGen, Inactive);
        assert(population.size() == (CUR_POPULATION_SIZE+numEvalsThisGen));
        
        
        
#pragma omp parallel for
        for (long k=0; k<numEvalsThisGen; k++){
            if (k == numEvalsThisGen-1){
                population[CUR_POPULATION_SIZE+k] = Individual(Unstructured);
                //printf(" %f(1)",score);
            } else if (k >= CUR_POPULATION_SIZE/2){
                Individual offspring = population[helper::myrand(CUR_POPULATION_SIZE/2)];
                offspring.mutate();
                population[CUR_POPULATION_SIZE+k] = offspring;
            } else {//crossover
                //ASSERT(population[2*k+1] != NULL, "k is " << k);
                Individual parent1 = population[2*k];
                Individual parent2 = population[2*k+1];
                population[CUR_POPULATION_SIZE+k] = parent1.crossover(parent2, true);
                //printf(" %f(%d)",score,age);
            }
        }
        
        
        totalNumEvals += numEvalsThisGen;
        
        //nonDominatedSort(population);
        std::sort(population.begin(),population.end(),Individual::compSpeed);
        if (population.size() > MAX_POP_SIZE){
            population.erase(population.begin()+MAX_POP_SIZE, population.end());
        }
        
        
        printf("\nPopulation: ");
        for (int i=0; i<population.size(); i++){
            Individual p = population[i];
            printf(" (%g, %d, %d, %g)", p.speed, p.age, p.rank, p.crowdingDistance);
            dotWriter.appendData(totalNumEvals, p.speed);
        }
        
        //Write diversity
        double averageDiversity = 0;
        for (Individual &p : population){
            for (Individual &q : population){
                averageDiversity += (p.network->distanceFrom(q.network));
            }
        }
        if (population.size()>1)
            averageDiversity /= (population.size() * (population.size()-1))/2;
        diversityWriter.appendData(averageDiversity, numEvalsThisGen);
        
        //Check for bestScore
        for (Individual &pt : population){
            if (pt.speed > bestScore){
                bestScore = pt.speed;
                std::cout << "\nNew Record: " << bestScore << "(" << (pt.age) << ")";
            }
        }
        printf("\n---------------------------\n");
        
        
    }
    
    dotWriter.writeTo("evolveNeuralNetworkPareto.csv");
    diversityWriter.writeTo("evolveNeuralNetworkParetoDiv.csv");
    
    NeuralNetwork *bestNN = (population[0]).getLastingNetwork();
#ifdef enable_graphics
    bestNN->writeTo("/Users/lahavlipson/Personal_Projects/Learn_OpenGL/myNN.csv");
#else
    bestNN->writeTo("/home/lol2107/bestNN.csv");
#endif
    Robot rob = starting_models::getArrow();
    rob.setNN(bestNN);
    
    return rob;
}




void learn::nonDominatedSort(std::vector<Individual> &population){
    std::vector<Individual *> front;
    for (Individual &p : population){
        p.dominationSet.clear();
        p.dominatedCount = 0;
        for (Individual &q : population){
            if (p.dominates(q))
                p.dominationSet.push_back(&q);
            else if (q.dominates(p))
                p.dominatedCount++;
        }
        if (p.dominatedCount == 0){
            p.rank = 1;
            front.push_back(&p);
        }
    }
    
    int frontCounter = 1;
    while (front.size() != 0){
        //Assign crowding distances
        double minSpeed, maxSpeed, minAge, maxAge;
        minSpeed = maxSpeed = minAge = maxAge = -1.0;
        for (Individual *p : front){
            p->crowdingDistance = 0;
            if (minAge < 0 || p->age < (int)minAge) minAge = p->age;
            if (maxAge < 0 || p->age > (int)maxAge) maxAge = p->age;
            if (minSpeed < 0 || p->speed < minSpeed) minSpeed = p->speed;
            if (maxSpeed < 0 || p->speed > maxSpeed) maxSpeed = p->speed;
        }
        if (front.size() >= 2 && maxSpeed - minSpeed > 0 && maxAge - minAge > 0){
            //speed
            std::sort(front.begin(), front.end(), Individual::compSpeedPtr);
            front[0]->crowdingDistance = FLT_MAX;
            front[front.size()-1]->crowdingDistance = FLT_MAX;
            for (int i=1; i<front.size()-1; i++)
                front[i]->crowdingDistance += (front[i+1]->speed - front[i-1]->speed)/(maxSpeed-minSpeed);
            
            //age
            std::sort(front.begin(), front.end(), Individual::compAgePtr);
            front[0]->crowdingDistance = FLT_MAX;
            front[front.size()-1]->crowdingDistance = FLT_MAX;
            for (int i=1; i<front.size()-1; i++)
                front[i]->crowdingDistance += (front[i+1]->age - front[i-1]->age)/(maxAge-minAge);
        }
        
        //Establish next front
        std::vector<Individual *> nextFront;
        assert(front.size() > 0);
        for (Individual *p : front){
            for (Individual *q : p->dominationSet){
                q->dominatedCount--;
                if (q->dominatedCount == 0){
                    q->rank = frontCounter+1;
                    nextFront.push_back(q);
                }
            }
        }
        frontCounter++;
        front = nextFront;
    }
    
    std::sort(population.begin(), population.end());
    assert(population.size() < 10 || population[0] < population[population.size()-1]);
}
