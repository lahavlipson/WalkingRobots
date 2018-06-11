//
//  robot.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/4/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "robot.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

float getRandFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

glm::vec3 Robot::getPointToSpawnMass(){
    const glm::vec3 centroid = calcCentroid();
    float MaxRadius = 0;
    for (auto it = masses.begin(); it != masses.end(); ++it){
        if (glm::distance(centroid, (*it)->pos) > MaxRadius){
            MaxRadius = glm::distance(centroid, (*it)->pos);
        }
    }
    MaxRadius += 10;
    
    glm::vec3 randomLoc;
    float closestDist;
    do {
        glm::vec3 randUnitVec(getRandFloat()-0.5f,getRandFloat()-0.5f,getRandFloat()-0.5f);
        randUnitVec = glm::normalize(randUnitVec);
        float randDistance = getRandFloat()*MaxRadius;
        std::cout << "randDistance: " << randDistance << std::endl;
        randomLoc = centroid + randUnitVec*randDistance;
        
        closestDist = FLT_MAX;
        for (auto it = masses.begin(); it != masses.end(); ++it){
            if (glm::distance((*it)->pos, randomLoc) < closestDist)
                closestDist = glm::distance((*it)->pos, randomLoc);
        }
    } while (closestDist < 1.2 || closestDist > 2 || randomLoc[1] < GROUND_LEVEL);
    
    return randomLoc;
}

void Robot::attachMass(int connections, Mass *m){
    
    if (masses.size() < connections)// maybe remove?
        return;
    
    if (m == NULL)
        m = new Mass(getPointToSpawnMass(), MASS_WEIGHT);
    
    Mass *closestMasses[connections];
    float closestMassesDists[connections];
    for (int i=0; i<connections; i++) {
        closestMasses[i] = NULL;
        closestMassesDists[i] = -1;
    }

    std::unordered_set<Mass *>::iterator it;
    for (int i = 0; i < connections; i++){
        for (it = masses.begin(); it != masses.end(); ++it){
            bool noConflicts = true;
            for (int j=0; j<i; j++)
                noConflicts = noConflicts && (*it != closestMasses[j]);
            float distToCheck = glm::distance((*it)->pos, m->pos);
            if (noConflicts & (closestMasses[i] == NULL || distToCheck < closestMassesDists[i])){
                closestMasses[i] = *it;
                closestMassesDists[i] = distToCheck;
            }
        }
    }
    
    addMass(m);
    for (int i = 0; i < connections; i++)
        addSpring(m,closestMasses[i],SPRING_CONST);
}

void Robot::removeMass(Mass *m){
    for (Spring *spr : m->springs){
        if (std::get<0>(springsMap[spr]) == m){
            std::get<1>(springsMap[spr])->springs.erase(spr);
        } else {
            std::get<0>(springsMap[spr])->springs.erase(spr);
        }
        springsMap.erase(spr);
        delete spr;
    }
    masses.erase(m);
    delete m;
}

void Robot::addMass(Mass *m){
    masses.insert(m);
}

void Robot::addSpring(Mass *m1, Mass *m2, float constant){
    static float phase = 0;
    const float amplitude = 0.2;
    const float frequency = 2.0;
    Spring *s = new Spring(m1->mp(),m2->mp(),constant,amplitude,phase,frequency);
    phase += 0.22;
    springsMap.insert({s,std::make_tuple(m1,m2)});
    m1->springs.insert(s);
    m2->springs.insert(s);
}

glm::vec3 Robot::calcCentroid(){
    glm::vec3 centroid = glm::vec3();
    for ( auto it = masses.begin(); it != masses.end(); ++it ){
        centroid = centroid + ((*it)->pos);
    }
    return centroid * (1/float(masses.size()));
}


std::vector<Spring *> Robot::getSprings(){
    std::vector<Spring *> output;
    for(auto it = springsMap.begin(); it != springsMap.end(); ++it) {
        output.push_back(it->first);
    }
    return output;
}

void Robot::simulate(){//std::vector<double> &rodBufferData){
    //simulate and update all the masses and springs.
    //Then update the rodBufferData for all the springs
    std::unique_lock<std::mutex> lck (*mtx,std::defer_lock);
    
    const float dt=0.001;
    float t = 0;
    while (!stopSim) {
        usleep(1000);//1000 is usual
        
        
        for (Mass *ms : masses){
            glm::vec3 force;
            force[1] = force[1] + ms->m*GRAVITY;
            force = force + pushForce;
            force = force - DRAG*ms->v;
            for (Spring *spr : ms->springs){
                glm::vec3 springForce = (spr->getVectorPointingToMass(&(ms->pos)))*(spr->calcForce(t));
                force = force + springForce;
            }
            if (ms->pos[1] < GROUND_LEVEL){
                const float GROUND_HARDNESS = 1000;
                const float GROUND_ELASTICITY = 0.9;
                float normalForceMag = GROUND_HARDNESS*(GROUND_LEVEL - (ms->pos[1]));
                force[1] = force[1] + normalForceMag;
                const float COEF_STATIC = 0.7;
                float maxStaticFric = normalForceMag*COEF_STATIC;
                if (pow(force[0],2)+pow(force[2],2) < pow(maxStaticFric,2)) {
                    ms->v = glm::vec3(0,ms->v[1],0);
                }
                ms->v = (ms->v)*GROUND_ELASTICITY;
            }
            
            glm::vec3 acceleration = force*float(1/ms->m);
            
            ms->v = ms->v + acceleration*(dt);
        }
        for (Mass *ms : masses){
            lck.lock();
            ms->pos = ms->pos + (ms->v)*dt;
            lck.unlock();
        }
        
        t += dt;
    }
    
}
