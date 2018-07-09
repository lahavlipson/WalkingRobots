//
//  robot.hpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/4/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef robot_h
#define robot_h

#include <vector>
#include "mass.h"
#include "spring.h"
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

#include <stdio.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tuple>
#include <unordered_map>
//#include <unordered_set>
#include <random>
#include <iostream>
#include "helper.h"
#include "neural_network.h"

const double GRAVITY = -9.81;
const double GROUND_LEVEL = 0.0;


class Robot {
    
public:
    
    glm::dvec3 getPointToSpawnMass();
    
    //Used for updating weights
    std::vector<Spring *> orderedListOfSprings;
    NeuralNetwork *network = NULL;
    
    inline void updateSprings(){
        if (network != NULL && orderedListOfSprings.size() > 0){
            assert(orderedListOfSprings.size() > 1);
            network->evaluate(orderedListOfSprings);
        } else
            printf("skipping spring updates\n");
    }
    
//public:
    std::vector<Mass *> masses;
    std::mutex *mtx = NULL;
    std::unordered_map<Spring *,std::tuple<Mass *, Mass *>> springsMap;
    glm::dvec3 pushForce;
    bool stopSim = false;
    bool wasSimulated = false;
    double frequency;
    
    inline void setNN(NeuralNetwork *nn){
        network = nn;
    }
    
    inline void setSpringVec(std::vector<Spring *> vec){
        orderedListOfSprings = vec;
    }
    
    Robot(std::mutex *mutex, double freq):mtx(mutex),frequency(freq){}
    
    Robot(const std::string encoding);
    
    Robot(){}
    
    Robot& operator=(const Robot& old_robot);
    
    Robot (const Robot &old_robot){
        *this = old_robot; 
    }
    
    glm::dvec3 calcCentroid();
    
    void addMass(Mass *m);
    
    void removeMass(Mass *m);
    
    void attachMass(int connections, Mass *m);
    
    void mutateMasses(){
        Mass *m = new Mass(getPointToSpawnMass(), MASS_WEIGHT);
        attachMass(4, m);
    }
    
    void mutateSprings(){
        int index = helper::myrand(springsMap.size());
        Spring *s = getSprings()[index];
        double r = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/(2*M_PI)));
        s->b = r;
    }
    
    std::vector<Spring *> getSprings();
    
    Spring *addSpring(Mass *m1, Mass *m2, double constant);
    
    double simulate(int uwait, int time = 1000000);
    
    void canDeRefMasses() const{
        for (Mass *mp : masses)
            Mass m = *mp;
    }
    
    ~Robot(){
        for (Mass *m : masses)
            delete m;
        std::vector<Spring *> springs = getSprings();
        for (int i=0; i<springs.size(); i++)
            delete springs[i];
    }
    
    friend std::ostream &operator<<(std::ostream &os, Robot &rob);
    
};

#endif /* robot_h */
