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
#include "robot_genome.h"

const double GRAVITY = -9.81;
const double GROUND_LEVEL = 0.0;
const double TIME_TO_RECORD = 40.0;

class Robot {
    
private:
    
    //Used for updating weights
    std::vector<Spring *> springList;
    NeuralNetwork *network = NULL;
    
    inline void updateSprings(){
        if (network){
            assert(springList.size() > 1);
            network->evaluate(springList);
        } /*else
            printf("Network is NULL\n");*/
    }
    
public:
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
    
    Robot(std::mutex *mutex, double freq):mtx(mutex),frequency(freq){}
    
    Robot(const RobotGenome genome);
    
    Robot(){}
    
    Robot& operator=(const Robot& old_robot);
    
    Robot (const Robot &old_robot){
        *this = old_robot; 
    }
    
    glm::dvec3 calcCentroid();
    
    void addMass(Mass *m);
    
    std::vector<Spring *> getSprings();
    
    Spring *addSpring(Mass *m1, Mass *m2, double constant);
    
    double simulate(int uwait, double time = 1000000);
    
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
