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
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <iostream>

const float GRAVITY = -9.81;
const float GROUND_LEVEL = 0.0;


class Robot {
    
private:
    
    glm::vec3 getPointToSpawnMass();
    
public:
    std::unordered_set<Mass *> masses;
    std::mutex *mtx = NULL;
    std::unordered_map<Spring *,std::tuple<Mass *, Mass *>> springsMap;
    glm::vec3 pushForce;
    bool stopSim = false;
    bool wasSimulated = false;
    float frequency;
    
    Robot(std::mutex *mutex, float freq):mtx(mutex),frequency(freq){}
    
    Robot(){} //breaks things :(
    
    Robot& operator=(const Robot& old_robot);
    
    Robot (const Robot &old_robot){
        *this = old_robot;
    }
    
    glm::vec3 calcCentroid();
    
    void addMass(Mass *m);
    
    void removeMass(Mass *m);
    
    void attachMass(int connections, Mass *m);
    
    void mutateMasses(){
        Mass *m = new Mass(getPointToSpawnMass(), MASS_WEIGHT);
        attachMass(3, m);
    }
    
    void mutateSprings(){
        int index = rand()%(springsMap.size());
        Spring *s = getSprings()[index];
        float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(2*M_PI)));
        s->b = r;
    }
    
    std::vector<Spring *> getSprings();
    
    void addSpring(Mass *m1, Mass *m2, float constant);
    
    float simulate(int uwait, int cycles = 1000000);
    
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
    
};

#endif /* robot_h */
