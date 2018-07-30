//
//  unstructured_neural_network.hpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef unstructured_neural_network_h
#define unstructured_neural_network_h

#include <stdio.h>
#include "Eigen/Dense"
#include <vector>
#include "spring.h"
#include "helper.h"
#include "multilayer_neural_network.h"
#include "neural_network.h"
#include "robot.h"
#include "starting_models.h"
#include "math.h"

const double SIM_TIME = 300;

class UnstructuredNeuralNetwork : public NeuralNetwork {
    
public:
    
    Eigen::MatrixXd weights;
    
    UnstructuredNeuralNetwork(long size){
       weights = Eigen::MatrixXd::Random(size, size);
    }
    
    UnstructuredNeuralNetwork (const UnstructuredNeuralNetwork &old_nn){
        *this = old_nn;
    }
    
    UnstructuredNeuralNetwork (std::vector<double> vec) {
        long size = std::sqrt(vec.size());
        assert(size*size == vec.size());
        weights = Eigen::MatrixXd(size, size);
        for (int i=0; i<size; i++) {
            for (int j=0; j<size; j++)
                weights(i,j) = vec[j*size + i];
        }
    }
    
    virtual void evaluate(std::vector<Spring *> &springs) const;
    void evaluate(Eigen::MatrixXd &layer, int num_propagate) const;
    
    UnstructuredNeuralNetwork crossover(UnstructuredNeuralNetwork &nn);
    
    UnstructuredNeuralNetwork &mutate();
    
    void writeTo(const char *filePath);
    
    virtual double calcSpeed();
    
    friend std::ostream &operator<<(std::ostream &os, UnstructuredNeuralNetwork &nn);
    
    double distanceFrom(const UnstructuredNeuralNetwork &nn) const;
    
    friend bool operator==(const UnstructuredNeuralNetwork&, const UnstructuredNeuralNetwork&);
    
private:
    Eigen::MatrixXd _vecForm() const;
    
};

#endif /* unstructured_neural_network_h */
