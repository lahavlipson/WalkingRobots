//
//  mass.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/3/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef mass_h
#define mass_h

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "spring.h"
#include <unordered_set>
#include <random>
#include <string>
#include <iostream>

const float MASS_WEIGHT = 0.2;

class Mass {
        
public:
    std::unordered_set<Spring *> springs;
    double a = 0.0;
    double m;
    glm::vec3 v = glm::vec3(0,0,0);
    glm::vec3 pos;
    
    Mass(glm::vec3 position, double mass):pos(position),m(mass){}
    
    glm::vec3 *mp(){
        return &pos;
    }
    
    //weight,pos[],springs
    friend std::ostream &operator<<(std::ostream &os, Mass &m) {
        os<<m.m<<"|"<<m.pos[0]<<","<<m.pos[1]<<","<<m.pos[2]<<"|";
        bool firstString = false;
        for (Spring *s : m.springs){
            if (firstString)
                os << ",";
            os << s;
            firstString = true;
        }
        return os;
    }
    
//    std::string str(){
//        std::string output = std::to_string(m)+"|"+std::to_string(pos[0])+","+std::to_string(pos[1])+","+std::to_string(pos[2])+"|";
//        for (Spring *s : springs)
//            output += (std::to_string(s) + ",");
//        return output.substr(0,string::length(output)-1);
//    }
    
    Mass (const Mass &old_mass){
        m = old_mass.m;
        pos = old_mass.pos;
        //Springs will be empty;
    }
    
};

#endif /* mass_h */
