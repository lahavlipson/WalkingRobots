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
#include <cmath>
#include <stdio.h>
#include <sstream>

// MARK: Robot(encoding)

Robot::Robot(const std::string encoding){
    std::vector<std::string> objects = helper::split(encoding, "$");
    const int numMasses = stoi(objects[0]);
    const int numSprings = stoi(objects[1]);
    assert(objects.size() == 3+numMasses+numSprings);
    
    for (int i=2;i<2+numMasses;i++){
        std::string massEncoding = objects[i];
        std::vector<std::string> massVec = helper::split(massEncoding, "|");
        assert(massVec.size()==3);
        const float weight = stof(massVec[0]);
        std::vector<std::string> posVec = helper::split(massVec[1], ",");
        assert(posVec.size()==3);
        const glm::vec3 pos(stof(posVec[0]),stof(posVec[1]),stof(posVec[2]));
        
    }
    
    //NEED TO FINISH
    
}

std::ostream &operator<<(std::ostream &os, Robot &rob){
    os << rob.masses.size() << "$" << rob.springsMap.size() << "$";
    for (Mass *m : rob.masses)
        os << *m << "$";
    for (Spring *s : rob.getSprings())
        os << *s << "$";
    return os;
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
        glm::vec3 randUnitVec(helper::myrandFloat()-0.5f,helper::myrandFloat()-0.5f,helper::myrandFloat()-0.5f);
        randUnitVec = glm::normalize(randUnitVec);
        float randDistance = helper::myrandFloat()*MaxRadius;
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

//MARK: - Spring Params
Spring *Robot::addSpring(Mass *m1, Mass *m2, float constant){
    static float phase = 0;
    const float amplitude = 0.05;
    Spring *s = new Spring(m1->mp(),m2->mp(),constant,0,0,0);
    phase += 0.22;
    springsMap.insert({s,std::make_tuple(m1,m2)});
    m1->springs.insert(s);
    m2->springs.insert(s);
    return s;
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
// MARK: operator=
Robot& Robot::operator=(const Robot& old_robot){
//    std::cout << "Copied!\n";
    springsMap.clear();
    masses.clear();
    frequency = old_robot.frequency;
    mtx = old_robot.mtx;
    network = old_robot.network;
    std::unordered_map<Spring *,Spring *> oldSpringToNew;
    std::unordered_map<Mass *,Mass *> oldMassToNew;
    for (Mass *m : old_robot.masses){
        oldMassToNew.insert({m,new Mass(*m)});
    }
    for (auto it = old_robot.springsMap.begin(); it != old_robot.springsMap.end(); it++){
        Spring *s = it->first;
        oldSpringToNew.insert({s,new Spring(*s)});
    }
    
    //orderedListOfSprings = old_robot.orderedListOfSprings;
    orderedListOfSprings.clear();
    for (int i=0; i<old_robot.orderedListOfSprings.size(); i++){
        Spring *old_spring = old_robot.orderedListOfSprings[i];
        orderedListOfSprings.push_back(oldSpringToNew[old_spring]);
    }
    
    
    for (Mass *m : old_robot.masses){
        Mass *newMass = oldMassToNew[m];
        masses.insert(newMass);
        for (Spring *s : m->springs)
            newMass->springs.insert(oldSpringToNew[s]);
    }
    for (auto it = old_robot.springsMap.begin(); it != old_robot.springsMap.end(); it++){
        Spring *s = it->first;
        Spring *newSpring = oldSpringToNew[s];
        Mass *newAttachedMass1 = oldMassToNew[std::get<0>(old_robot.springsMap.at(s))];
        Mass *newAttachedMass2 = oldMassToNew[std::get<1>(old_robot.springsMap.at(s))];
        springsMap.insert({newSpring,std::make_tuple(newAttachedMass1,newAttachedMass2)});
        newSpring->p1 = newAttachedMass1->mp();
        newSpring->p2 = newAttachedMass2->mp();
    }
    
    return *this;
}

float Robot::simulate(int uwait, int time){//std::vector<double> &rodBufferData){
    //simulate and update all the masses and springs.
    //Then update the rodBufferData for all the springs
    //std::unique_lock<std::mutex> lck (*mtx,std::defer_lock);
    assert(!wasSimulated);
    wasSimulated = true;
    
    glm::vec3 startingPos;// = calcCentroid();
    bool alreadySetStartingPos = false;
    const float TIME_TO_RECORD = 10.0f;
    
    const float dt=0.001;
    float t = 0;
    while (!stopSim && t < time){//(M_PI*float(2*cycles)/frequency)) {
        if (uwait > 0)
            usleep(uwait);//1000 is normal
        
        assert(time > TIME_TO_RECORD);
        if (!alreadySetStartingPos && t > TIME_TO_RECORD){
            startingPos = calcCentroid();
            alreadySetStartingPos = true;
        }
        
        static unsigned int updateCounter = 0;
        if (updateCounter%50 == 0)
            updateSprings();
        updateCounter++;
        
        for (Mass *ms : masses){
            glm::vec3 force;
            force[1] = force[1] + ms->m*GRAVITY;
            force = force + pushForce;
            const float DRAG = 0.01; //(0 = no drag)
            force = force - ms->v*DRAG*(std::pow(glm::length(ms->v),1.0f));
            for (Spring *spr : ms->springs){
                glm::vec3 springForce = (spr->getVectorPointingToMass(&(ms->pos)))*(spr->calcForce(t));
                force = force + springForce;
            }
            if (ms->pos[1] < GROUND_LEVEL){
                const float GROUND_HARDNESS = 1000;
                const float GROUND_ELASTICITY = 0.93;
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
            
            //keep this in!!! just removed temporarily
            const float MAX_SPEED = 1;
            if (glm::length(ms->v) > MAX_SPEED)
                ms->v = glm::normalize(ms->v)*MAX_SPEED;
        }
        for (Mass *ms : masses){
           // lck.lock();
            ms->pos = ms->pos + (ms->v)*dt;
           // lck.unlock();
        }
        
        t += dt;
    }
    
    return glm::length(calcCentroid() - startingPos);
    
}
