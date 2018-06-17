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



namespace learn {
    
    
    Robot *getTetrahedron(){
        std::mutex mtx;           // mutex for critical section
        std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
        Robot *rob = new Robot(&mtx, 2.0f);
        
        std::vector<Mass *> masses;
        masses.push_back(new Mass(glm::vec3(0,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(2,0,0),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(1,0,-2),MASS_WEIGHT));
        masses.push_back(new Mass(glm::vec3(1,1,-0.8),MASS_WEIGHT));
        for (Mass *m : masses)
            rob->addMass(m);
        
        for (int i=0; i<masses.size(); i++){
            for (int j=i+1; j<masses.size(); j++){
                rob->addSpring(masses[i],masses[j],SPRING_CONST);
            }
        }
        return rob;
    }
    
    Robot *getCube(){
        std::mutex mtx;           // mutex for critical section
        std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
        Robot *rob = new Robot(&mtx, 2.0f);
        
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
            rob->addMass(m);
        
        for (int i=0; i<masses.size(); i++){
            for (int j=i+1; j<masses.size(); j++){
                //Spring *spr = new Spring(masses[i]->mp(),masses[j]->mp(),8);
                rob->addSpring(masses[i],masses[j],SPRING_CONST);
            }
        }
        
        return rob;
        
    }


Robot *hillClimber(int generations){
    
    int numgens = 0;
    
    Robot bestSoFar = *getTetrahedron();

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
    
    return new Robot(bestSoFar);
}

}


#endif /* learn_h */
