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


Robot *hillClimber(int generations){
    
    int numgens = 0;
    
    Robot bestSoFar = *getTetrahedron();

    float bestDist = -1;
    while (numgens < generations) {
        
        std::cout << "gen: " << numgens << std::endl;
        
        std::vector<std::tuple<Robot,float>> scores;
        
        #pragma omp parallel
        {
            Robot mutatedBot = bestSoFar;
            if (rand()%2 == 0)
                mutatedBot.mutateMasses();
            else
                mutatedBot.mutateSprings();
            const Robot mutatedBotConst = mutatedBot;
            
            float distance = mutatedBot.simulate(0,20);
            
            std::tuple<Robot,float> resultTup = std::make_tuple(mutatedBotConst,distance);
            scores.push_back(resultTup);
            
        }
        assert(scores.size() == 4);
        
        for (auto tup : scores){
            float distance = std::get<1>(tup);
            const Robot mutatedBotConst = std::get<0>(tup);
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
