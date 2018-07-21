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
        const double weight = stof(massVec[0]);
        std::vector<std::string> posVec = helper::split(massVec[1], ",");
        assert(posVec.size()==3);
        const glm::dvec3 pos(stof(posVec[0]),stof(posVec[1]),stof(posVec[2]));
        
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

glm::dvec3 Robot::getPointToSpawnMass(){
    const glm::dvec3 centroid = calcCentroid();
    double MaxRadius = 0;
    for (auto it = masses.begin(); it != masses.end(); ++it){
        if (glm::distance(centroid, (*it)->pos) > MaxRadius){
            MaxRadius = glm::distance(centroid, (*it)->pos);
        }
    }
    MaxRadius += 10;
    
    glm::dvec3 randomLoc;
    double closestDist;
    do {
        glm::dvec3 randUnitVec(helper::myranddouble()-0.5,helper::myranddouble()-0.5,helper::myranddouble()-0.5);
        randUnitVec = glm::normalize(randUnitVec);
        double randDistance = helper::myranddouble()*MaxRadius;
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
    double closestMassesDists[connections];
    for (int i=0; i<connections; i++) {
        closestMasses[i] = NULL;
        closestMassesDists[i] = -1;
    }

    //std::unordered_set<Mass *>::iterator it;
    for (int i = 0; i < connections; i++){
        for (Mass *m : masses){
            bool noConflicts = true;
            for (int j=0; j<i; j++)
                noConflicts = noConflicts && (m != closestMasses[j]);
            double distToCheck = glm::distance(m->pos, m->pos);
            if (noConflicts & (closestMasses[i] == NULL || distToCheck < closestMassesDists[i])){
                closestMasses[i] = m;
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
    //masses.erase(m);
    masses.erase(std::remove(masses.begin(), masses.end(), m), masses.end());
    delete m;
}

void Robot::addMass(Mass *m){
    masses.push_back(m);
}

//MARK: - Spring Params
Spring *Robot::addSpring(Mass *m1, Mass *m2, double constant){
    static double phase = 0;
    const double amplitude = 0.05;
    Spring *s = new Spring(m1->mp(),m2->mp(),constant,0,0,0);
    phase += 0.22;
    springsMap.insert({s,std::make_tuple(m1,m2)});
    m1->springs.insert(s);
    m2->springs.insert(s);
    return s;
}

glm::dvec3 Robot::calcCentroid(){
    glm::dvec3 centroid = glm::dvec3();
    for ( auto it = masses.begin(); it != masses.end(); ++it ){
        centroid = centroid + ((*it)->pos);
    }
    return centroid * (1/double(masses.size()));
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
        masses.push_back(newMass);
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

double Robot::simulate(int uwait, double time){//std::vector<double> &rodBufferData){
    //simulate and update all the masses and springs.
    //Then update the rodBufferData for all the springs
    //std::unique_lock<std::mutex> lck (*mtx,std::defer_lock);
    assert(!wasSimulated);
    wasSimulated = true;
    
    glm::dvec3 startingPos;// = calcCentroid();
    bool alreadySetStartingPos = false;
    
    const double dt=0.001;
    double t = 0;
    
    int updateCounter = 0;
    assert(time > TIME_TO_RECORD);
    while (!stopSim && t < time){//(M_PI*double(2*cycles)/frequency)) {
        if (uwait > 0)
            usleep(uwait);//1000 is normal
        
        if (!alreadySetStartingPos && t > TIME_TO_RECORD){
            startingPos = calcCentroid();
            alreadySetStartingPos = true;
            //printf("Rec! %f",t);
        }
//        if (int(t*1000.0)%200 == 0)
//            printf("t is %f\n",t);
        
        if (updateCounter%20 == 0){
            updateSprings();
            //printf("%^&*");
        }
        updateCounter++;
        
        for (Mass *ms : masses){
//            if (updateCounter%100 == 0)
//            PRINT_F(ms->pos[1]);
            glm::dvec3 force;
            force[1] = force[1] + ms->m*GRAVITY;
            force = force + pushForce;
            const double DRAG = 0.01; //(0 = no drag)
            force = force - ms->v*DRAG*(std::pow(glm::length(ms->v),1.0));
            for (Spring *spr : ms->springs){
                glm::dvec3 springForce = (spr->getVectorPointingToMass(&(ms->pos)))*(spr->calcForce(t));
                force = force + springForce;
            }
            if (ms->pos[1] < GROUND_LEVEL){
                const double GROUND_HARDNESS = 1000;
                const double GROUND_ELASTICITY = 0.93;
                double normalForceMag = GROUND_HARDNESS*(GROUND_LEVEL - (ms->pos[1]));
                force[1] = force[1] + normalForceMag;
                const double COEF_STATIC = 0.7;
                double maxStaticFric = normalForceMag*COEF_STATIC;
                if (pow(force[0],2)+pow(force[2],2) < pow(maxStaticFric,2)) {
                    ms->v = glm::dvec3(0,ms->v[1],0);
                }
                ms->v = (ms->v)*GROUND_ELASTICITY;
            }
            
            glm::dvec3 acceleration = force*double(1/ms->m);
            
            ms->v = ms->v + acceleration*(dt);
            
            //keep this in!!!
            const double MAX_SPEED = 4.0;
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
    
    //PRINT_F(glm::length(calcCentroid() - startingPos));
    return glm::length(calcCentroid() - startingPos)/(time - TIME_TO_RECORD);
    
}
