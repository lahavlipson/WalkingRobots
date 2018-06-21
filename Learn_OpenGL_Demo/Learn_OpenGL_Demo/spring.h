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

const float SPRING_CONST = 8000.0;

//void pv3(glm::vec3 v){
//    std::cout << "(" << v[0] << ", " << v[0] << ", " << v[0] << ")\n";
//}

class Spring {
    
private:
    float l_0;
    
    inline float calcRestLength(float t){
        //std::cout << t << std::endl;
        
        return l_0 + a*sinf(w*t + b);
    }
    
public:
    glm::vec3 *p1 = NULL;
    glm::vec3 *p2 = NULL;
    float k;
    
    //For oscillation
    float a;
    float b;//won't change anything rn
    float w;
    
    Spring(glm::vec3 *pos1, glm::vec3 *pos2, float constant, float amplitude, float phase, float frequency):p1(pos1),p2(pos2),k(constant), a(amplitude), b(phase), w(frequency){
        const float currentLength = glm::length(*pos2 - *pos1);
        const float calculatedRestLength = a*sinf(b);
        l_0 = currentLength - calculatedRestLength;
    }
    
    Spring (const Spring &old_spring){
        k = old_spring.k;
        a = old_spring.a;
        b = old_spring.b;
        w = old_spring.w;
        l_0 = old_spring.l_0;
        //Pointers will be null;
    }
    
    inline double calcLength(){
        return glm::length(*p2-*p1);
    }
    
    inline float calcForce(float t){
        return -k*float(calcLength() - calcRestLength(t));
    }
    
    inline glm::vec3 getVectorPointingToMass(glm::vec3 *pos_ptr){
        assert(p1 != NULL && p2 != NULL);
        glm::vec3 v = glm::normalize(*p1-*p2);
        if (pos_ptr == p1)
            return v;
        return v*float(-1.0);
    }
    
    friend std::ostream &operator<<(std::ostream &os, Spring &s) {
        return os<<s.l_0<<"|"<<s.k<<"|"<<s.a<<"|"<<s.b<<"|"<<s.w<<"|"<<s.p1<<","<<s.p2;
    }

};

#endif /* spring_h */
