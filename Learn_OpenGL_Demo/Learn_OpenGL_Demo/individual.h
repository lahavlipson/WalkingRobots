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

enum IndividualType { Unstructured, Layered, Inactive};

class Individual {
    
public:
    
    //For sorting
    std::vector<Individual *> dominationSet;
    int dominatedCount;
    double crowdingDistance;
    int rank = -300;
    
    //Properties
    NeuralNetwork *network = nullptr;
    double speed;
    int age;
    
    IndividualType type = Inactive;
    
    Individual(NeuralNetwork *n, double s, int a, IndividualType t):network(n),speed(s),age(a),type(t){}
    
    Individual(IndividualType type);
    
    Individual crossover(Individual ind, bool withMutate = false);
    
    void mutate();
    
    bool dominates(Individual ind) const;
    
    //friend bool operator< (Individual const& ind_a, Individual const& ind_b);
    bool operator < (Individual const& ind) const;
    
    Individual (const Individual &old_robot){
        *this = old_robot;
    }
    
    Individual& operator= (const Individual &ind){
        
        if (ind.type == Unstructured){
            if (network) delete network;
            network = new UnstructuredNeuralNetwork(*(dynamic_cast<UnstructuredNeuralNetwork *>(ind.network)));
        } else if (ind.type == Layered){
            if (network) delete network;
            network = new MultilayerNeuralNetwork(*(dynamic_cast<MultilayerNeuralNetwork *>(ind.network)));
        }
        speed = ind.speed;
        age = ind.age;
        crowdingDistance = ind.crowdingDistance;
        rank = ind.rank;
        type = ind.type;
        return *this;
    }
    
    NeuralNetwork *getLastingNetwork(){
        if (type == Unstructured)
            return new UnstructuredNeuralNetwork(*(dynamic_cast<UnstructuredNeuralNetwork *>(network)));
        else if (type == Layered)
            return new MultilayerNeuralNetwork(*(dynamic_cast<MultilayerNeuralNetwork *>(network)));
        else
            return nullptr;
    }
    
    static bool compSpeed(Individual *i, Individual *j){
        return i->speed < j->speed;
    }
    
    static bool compAge(Individual *i, Individual *j){
        return i->age < j->age;
    }
    
    virtual ~Individual(){
        if (network)
            delete network;
    }
    
};

#endif /* individual_h */
