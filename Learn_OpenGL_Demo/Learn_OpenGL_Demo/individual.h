//
//  individual.hpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/26/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef individual_h
#define individual_h

#include <stdio.h>
#include "unstructured_neural_network.h"

class Individual {
    
public:
    
    //For sorting
    std::vector<Individual *> dominationSet;
    int dominatedCount;
    double crowdingDistance;
    int rank = -300;
    
    //Properties
    UnstructuredNeuralNetwork network;
    double speed;
    int age;
    
    
    
    Individual(UnstructuredNeuralNetwork n, double s, int a):network(n),speed(s),age(a){}
    
    Individual(bool activate);
    
    Individual crossover(Individual ind, bool withMutate = false);
    
    void mutate();
    
    bool dominates(Individual ind) const;
    
    //friend bool operator< (Individual const& ind_a, Individual const& ind_b);
    bool operator < (Individual const& ind) const;
    
    static bool compSpeed(Individual *i, Individual *j){
        return i->speed < j->speed;
    }
    
    static bool compAge(Individual *i, Individual *j){
        return i->age < j->age;
    }
    
};

#endif /* individual_h */
