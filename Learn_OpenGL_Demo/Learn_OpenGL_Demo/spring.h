//
//  spring.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/3/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef spring_h
#define spring_h

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>

const float SPRING_CONST = 800.0;

//void pv3(glm::vec3 v){
//    std::cout << "(" << v[0] << ", " << v[0] << ", " << v[0] << ")\n";
//}

class Spring {
    
private:
    float l_0;
    
    float calcRestLength(float t){
        //std::cout << t << std::endl;
        
        return l_0 + a*sinf(w*t + b);
    }
    
public:
    glm::vec3 *p1;
    glm::vec3 *p2;
    float k;
    
    //For oscillation
    float a;
    float b;//won't change anything rn
    float w;
    
//    Spring(glm::vec3 *pos1, glm::vec3 *pos2, float constant):p1(pos1),p2(pos2),k(constant){
//        l_0 = glm::length(*pos2 - *pos1);
//    }
    
    Spring(glm::vec3 *pos1, glm::vec3 *pos2, float constant, float amplitude, float phase, float frequency):p1(pos1),p2(pos2),k(constant), a(amplitude), b(phase), w(frequency){
        const float currentLength = glm::length(*pos2 - *pos1);
        const float calculatedRestLength = a*sinf(b);
        l_0 = currentLength - calculatedRestLength;
        
    }
    
    double calcLength(){
        return glm::length(*p2-*p1);
    }
    
    float calcForce(float t = -1){
        if (t<0)
            t = -b/w;
       // std::cout << calcLength() << " " << l_0 << "\n";
        return -k*float(calcLength() - calcRestLength(t));
        
    }
    
    glm::vec3 getVectorPointingToMass(glm::vec3 *pos_ptr){
        glm::vec3 v = glm::normalize(*p1-*p2);
        if (pos_ptr == p1)
            return v;
        return v*float(-1.0);
    }

};

#endif /* spring_h */
