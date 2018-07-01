//
//  starting_models.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/23/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "starting_models.h"


Robot starting_models::getCuboid(int x, int y, int z) {
    std::mutex mtx;           // mutex for critical section
    std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
    Robot rob(&mtx, 2.0f);
    
    Mass *massPointers[x][y][z];
    for (int i=0; i<x; i++){
        for (int j=0; j<y; j++){
            for (int k=0; k<z; k++){
                massPointers[i][j][k] = new Mass(glm::vec3(i,j,k)*ROD_LENGTH,MASS_WEIGHT);
                rob.addMass(massPointers[i][j][k]);
            }
        }
    }
    for (int i=0; i<x; i++){
        for (int j=0; j<y; j++){
            for (int k=0; k<z; k++){
                if (i<x-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j][k],SPRING_CONST);
                if (j<y-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i][j+1][k],SPRING_CONST);
                if (k<z-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i][j][k+1],SPRING_CONST);
                
                if (i<x-1 && j<y-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j+1][k],SPRING_CONST);
                if (i<x-1 && j>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j-1][k],SPRING_CONST);
                if (i<x-1 && k<z-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j][k+1],SPRING_CONST);
                if (i<x-1 && k>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j][k-1],SPRING_CONST);
                if (k>0 && j>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i][j-1][k-1],SPRING_CONST);
                if (k<z-1 && j>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i][j-1][k+1],SPRING_CONST);
                
                if (i<x-1 && j<y-1 && k<z-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j+1][k+1],SPRING_CONST);
                if (i<x-1 && j>0 && k<z-1)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j-1][k+1],SPRING_CONST);
                if (i<x-1 && j<y-1 && k>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j+1][k-1],SPRING_CONST);
                if (i<x-1 && j>0 && k>0)
                    rob.addSpring(massPointers[i][j][k],massPointers[i+1][j-1][k-1],SPRING_CONST);
            }
        }
    }
    
    return rob;
    
}

Robot starting_models::getTetrahedron(){
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

Robot starting_models::getCube(){
    std::mutex mtx;           // mutex for critical section
    std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
    Robot rob(&mtx, 2.0f);
    
    std::vector<Mass *> masses;
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

Robot starting_models::getTetroid(){
    std::mutex mtx;           // mutex for critical section
    std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
    Robot rob(&mtx, 2.0f);
    
    for (float k=0;k<2;k++){
        for (int i=0; i<3; i++)
            rob.addMass(new Mass(glm::vec3(i+0.5,k*1.4,0)*ROD_LENGTH,MASS_WEIGHT));
        for (int i=0; i<4; i++)
            rob.addMass(new Mass(glm::vec3(i,k*1.4,-1)*ROD_LENGTH,MASS_WEIGHT));
        for (int i=0; i<3; i++)
            rob.addMass(new Mass(glm::vec3(i+0.5,k*1.4,-2)*ROD_LENGTH,MASS_WEIGHT));
        
        for (int j=0; j<2 && k<1; j++){
            for (int i=0; i<3; i++)
                rob.addMass(new Mass(glm::vec3(i+0.5,0.7+k*1.4,-0.5-j)*ROD_LENGTH,MASS_WEIGHT));
        }
    }
    
    
    std::vector<std::string> massPairs;
    std::vector<Spring *> springList;
    for (Mass *m1: rob.masses){
        for (Mass *m2: rob.masses){
            std::stringstream ss1;
            ss1 << m1 << m2;
            const bool ss1Exists = std::find(massPairs.begin(), massPairs.end(), ss1.str()) != massPairs.end();
            std::stringstream ss2;
            ss2 << m2 << m1;
            const bool ss2Exists = std::find(massPairs.begin(), massPairs.end(), ss2.str()) != massPairs.end();
            if (m1 != m2 && !ss1Exists && !ss2Exists && glm::distance(m2->pos,m1->pos) <= 1.8f){
                Spring *s = rob.addSpring(m1,m2,SPRING_CONST);
                springList.push_back(s);
                massPairs.push_back(ss1.str());
                massPairs.push_back(ss2.str());
            }
        }
    }
    
    assert(springList.size() > 1);
    
    rob.setSpringVec(springList);
    
    return rob;
}
