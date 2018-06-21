//
//  learn.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/12/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef learn_h
#define learn_h
#include "mass.h"
#include "spring.h"
#include "robot.h"
#include <tuple>
#include "omp.h"
#include <algorithm>
#include <vector>
#include "helper.h"


namespace learn {
    
    
    Robot getTetrahedron(){
        std::mutex mtx;           // mutex for critical section
        std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
        Robot rob(&mtx, 2.0f);
        
        std::vector<Mass *> masses;
        masses.push_back(new Mass(glm::vec3(0,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(2,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(1,0,-2),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(1,1,-0.8),MASS_WEIGHT));
        for (Mass *m : masses)
            rob.addMass(m);
        
        for (int i=0; i<masses.size(); i++){
            for (int j=i+1; j<masses.size(); j++){
                rob.addSpring(masses[i],masses[j],SPRING_CONST);
            }
        }
        return rob;
    }
    
    Robot getCube(){
        std::mutex mtx;           // mutex for critical section
        std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
        Robot rob(&mtx, 2.0f);
        
        std::vector<Mass *> masses;
        const float ROD_LENGTH = 1.5;
        masses.push_back(new Mass(glm::vec3(0,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(ROD_LENGTH,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(0,ROD_LENGTH,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(ROD_LENGTH,ROD_LENGTH,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(0,0,-ROD_LENGTH),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(ROD_LENGTH,0,-ROD_LENGTH),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(0,ROD_LENGTH,-ROD_LENGTH),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(ROD_LENGTH,ROD_LENGTH,-ROD_LENGTH),MASS_WEIGHT));
        
        for (Mass *m : masses)
            rob.addMass(m);
        
        for (int i=0; i<masses.size(); i++){
            for (int j=i+1; j<masses.size(); j++){
                //Spring *spr = new Spring(masses[i]->mp(),masses[j]->mp(),8);
                rob.addSpring(masses[i],masses[j],SPRING_CONST);
            }
        }
        
        return rob;
        
    }


Robot hillClimber(int generations){
    
    int numgens = 0;
    
    Robot bestSoFar = getTetrahedron();

    float bestDist = -1;
    while (numgens < generations) {
        
        std::cout << "gen: " << numgens << std::endl;
        
        std::vector<float> scores;
        std::vector<Robot> scoredBots;
        
         assert(scoredBots.size()==0);
        const int NUMBER_CORES_ESTIMATE = 4;
        for (int i=0;i<NUMBER_CORES_ESTIMATE;i++){
            Robot r;
            scoredBots.push_back(r);
            scores.push_back(-1.0f);
        }
        
        #pragma omp parallel
        {
            
            #pragma omp for
            for (int i=0;i<omp_get_num_threads();i++){
                Robot mutatedBot = bestSoFar;
                if (i%2 == 0)
                    mutatedBot.mutateMasses();
                else
                    mutatedBot.mutateSprings();
                const Robot mutatedBotConst = mutatedBot;
                
                float distance = mutatedBot.simulate(0,20);
                
                assert(scores.size() > i);
                scores[i] = distance;
                scoredBots[i] = mutatedBotConst;
            }
            
        }

        for (int i=0;i<scores.size();i++){
            float distance = scores[i];
            const Robot mutatedBotConst = scoredBots[i];
            mutatedBotConst.canDeRefMasses();//sanity check
            if (distance > bestDist){
                bestDist = distance;
                bestSoFar = mutatedBotConst;
                std::cout << "New Record: " << distance << std::endl;
            }
        }
        
        

        numgens++;
    }
    
    return bestSoFar;
}
    
    Robot poolClimber(int generations){
        const int POOL_SIZE = 32;
        std::vector<std::tuple<Robot,float>> population;
        
        //initialize population (all tetrahedrons)
        Robot tetra = getTetrahedron();
        const Robot constTetra = tetra;
        const float tetraScore = tetra.simulate(0,8);
        int r=6;
        for (int i=0; i<POOL_SIZE; i++){
            std::tuple<Robot,float> tup(constTetra, tetraScore);
            population.push_back(tup);
        }
        assert(population.size() == POOL_SIZE);
        
        int numgens = 0;
        float bestScore = 0;
        while (numgens < generations){
            
            std::cout << "gen: " << numgens << std::endl;
            
            //Rank
            int i, j, max_idx;
            for (i = 0; i < POOL_SIZE-1; i++)
            {
                // Find the maximum element in unsorted array
                max_idx = i;
                for (j = i+1; j < POOL_SIZE; j++)
                    if (std::get<1>(population[j]) > std::get<1>(population[max_idx]))
                        max_idx = j;
                
                // Swap the found maximum element with the first element
                
                population[max_idx].swap(population[i]);
                
            }
            
            //Replace
            #pragma omp parallel for
            for (int i=POOL_SIZE*0.5;i<POOL_SIZE;i++){
                Robot rob = std::get<0>(population[i - POOL_SIZE*0.5]);
                if (i%2 == 0)
                    rob.mutateMasses();
                else
                    rob.mutateSprings();
                const Robot constRob = rob;
                const float score = rob.simulate(0,20);
                std::tuple<Robot,float> tup(constRob, score);
                population[i] = tup;
            }
            
            //Update top score
            if (std::get<1>(population[0]) > bestScore){
                bestScore = std::get<1>(population[0]);
                std::cout << "New Record: " << bestScore << std::endl;
            }
            
            numgens+=1;
        }
        
        return std::get<0>(population[0]);
        
    }
    
    Mass *getNextPoint(Mass *m1, Mass *m2, Mass *m3){
        return helper::getCenterVec(m1,m2,m3);
    }
    
    Robot synethsize(int numMassesAdded){
        Robot rob = getTetrahedron();
        std::vector<Mass *> masses;
        for (Mass *m : rob.masses)
            masses.push_back(m);

        int addedMasses = 0;
        while (addedMasses < numMassesAdded) {
            Mass *m = getNextPoint(masses[0],masses[1],masses[2]);
            masses.insert(masses.begin(),m);
            rob.attachMass(3, m);
            addedMasses++;
        }

        
        return rob;
    }

}


#endif /* learn_h */
