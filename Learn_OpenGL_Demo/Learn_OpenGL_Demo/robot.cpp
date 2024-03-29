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

Robot::Robot(RobotGenome genome){
    std::unordered_map<int,std::tuple<Mass *, Mass *>> idMappings;
    for (int i=0; i<genome.size(); i++){
        GenomeBase genomeBase = genome[i];
        std::cout << genomeBase << "\n";
        if (genomeBase.isLateral()){
            Mass *m1 = new Mass(genomeBase.getLoc(), MASS_WEIGHT);
            Mass *m2 = new Mass(genomeBase.getSibLoc(), MASS_WEIGHT);
            idMappings.insert({genomeBase.ID, std::make_tuple(m1,m2)});
            addMass(m1);addMass(m2);
            for (int neighbor_id : genomeBase.neighbors){//for all neighbors
                assert(idMappings.find(neighbor_id) != idMappings.end());
                Mass *neighborMass1 = std::get<0>(idMappings.at(neighbor_id));
                Mass *neighborMass2 = std::get<1>(idMappings.at(neighbor_id));
                
                if (neighbor_id == genomeBase.ID){ //special case
                    printf("#1\n");
                    addSpring(m1,m2, SPRING_CONST);
                } else if (neighborMass2) {//neighbor is also lateral
                    printf("#2\n");
                    addSpring(m1,neighborMass1, SPRING_CONST);
                    addSpring(m2,neighborMass2, SPRING_CONST);
                } else { //neighbor is center
                    printf("#3\n");
                    addSpring(m1,neighborMass1, SPRING_CONST);
                    addSpring(m2,neighborMass1, SPRING_CONST);
                }
            }
        } else { //current genome is center
            Mass *m = new Mass(genomeBase.getLoc(), MASS_WEIGHT);
            idMappings.insert({genomeBase.ID, std::make_tuple(m,nullptr)});
            addMass(m);
            for (int neighbor_id : genomeBase.neighbors){//for all neighbors
                assert(idMappings.find(neighbor_id) != idMappings.end());
                assert(neighbor_id != genomeBase.ID);
                Mass *neighborMass1 = std::get<0>(idMappings.at(neighbor_id));
                Mass *neighborMass2 = std::get<1>(idMappings.at(neighbor_id));
                
                if (neighborMass2) {//neighbor is lateral
                    printf("#4\n");
                    addSpring(m,neighborMass1, SPRING_CONST);
                    addSpring(m,neighborMass2, SPRING_CONST);
                } else { //neighbor is also center
                    printf("#5\n");
                    addSpring(m,neighborMass1, SPRING_CONST);
                }
            }
        }
    }
}

std::ostream &operator<<(std::ostream &os, Robot &rob){
    os << rob.masses.size() << "$" << rob.springsMap.size() << "$";
    for (Mass *m : rob.masses)
        os << *m << "$";
    for (Spring *s : rob.getSprings())
        os << *s << "$";
    return os;
}

void Robot::addMass(Mass *m){
    masses.push_back(m);
}

//MARK: - Spring Params
Spring *Robot::addSpring(Mass *m1, Mass *m2, double constant){
    Spring *s = new Spring(m1->mp(),m2->mp(),constant,0,0,0);
    springsMap.insert({s,std::make_tuple(m1,m2)});
    springList.push_back(s);
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
    return springList;
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
    
    springList.clear();
    for (int i=0; i<old_robot.springList.size(); i++){
        Spring *old_spring = old_robot.springList[i];
        springList.push_back(oldSpringToNew[old_spring]);
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
            const double MAX_SPEED = 2.5;
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
