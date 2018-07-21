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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>

const double SPRING_CONST = 8000.0;

//void pv3(glm::dvec3 v){
//    std::cout << "(" << v[0] << ", " << v[0] << ", " << v[0] << ")\n";
//}

class Spring {
    
private:
    
    inline double calcRestLength(double t){
        return l_0 + a*sinf(w*t + b);
    }
    
public:
    glm::dvec3 *p1 = NULL;
    glm::dvec3 *p2 = NULL;
    double k;
    double l_0;
    
    //For oscillation
    double a=0;
    double b=0;//won't change anything rn
    double w=0;
    
    Spring(glm::dvec3 *pos1, glm::dvec3 *pos2, double constant, double amplitude, double phase, double frequency):p1(pos1),p2(pos2),k(constant), a(amplitude), b(phase), w(frequency){
        const double currentLength = glm::length(*pos2 - *pos1);
        const double calculatedRestLength = a*sinf(b);
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
    
    virtual inline glm::dvec3 calcCenter() const{
        return (*p1 + *p2)*0.5;
    }
    
    virtual inline double calcLength() const{
        return glm::length(*p2-*p1);
    }
    
    inline double calcForce(double t=0){
        return -k*double(calcLength() - l_0);//calcRestLength(t));
    }
    
    inline glm::dvec3 getVectorPointingToMass(glm::dvec3 *pos_ptr){
        assert(p1 != NULL && p2 != NULL);
        glm::dvec3 v = glm::normalize(*p1-*p2);
        if (pos_ptr == p1)
            return v;
        return v*double(-1.0);
    }
    
    friend std::ostream &operator<<(std::ostream &os, Spring &s) {
        return os<<s.l_0<<"|"<<s.k<<"|"<<s.a<<"|"<<s.b<<"|"<<s.w<<"|"<<s.p1<<","<<s.p2;
    }

};

class DummySpring : public Spring {
    
public:
    
    glm::dvec3 pos;
    
    DummySpring (const Spring &old_spring):Spring(old_spring){
        const double r2 = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/0.8));
        l_0 = 1.1+r2;
        
        pos = old_spring.calcCenter();
    }
    
    virtual inline double calcLength() const{
        return l_0;
    }
    
    virtual inline glm::dvec3 calcCenter() const{
        return pos;
    }
};

#endif /* spring_h */
