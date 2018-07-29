//
//  neural_network.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef multilayer_neural_network_h
#define multilayer_neural_network_h

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Eigen/Dense"
#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <cmath>
#include "helper.h"
#include "neural_network.h"
#include "spring.h"
#include <string>
#include <fstream>

namespace nn_helper {
    
    inline Eigen::MatrixXd &activate(Eigen::MatrixXd &vec, double lift = 0.0, double height = 1.0, double width = 1.0) {
        if (vec.rows() == 1){
            for (int i=0; i<vec.cols(); i++)
                vec(0,i) = height/(1.0+exp(-1.0*(vec(0,i)/width))) + lift;
        } else if (vec.cols() == 1) {
            for (int i=0; i<vec.rows(); i++)
                vec(i,0) = height/(1.0+exp(-1.0*(vec(i,0)/width))) + lift;
        }
        return vec;
    }
    
    inline Eigen::MatrixXd appendBias(Eigen::MatrixXd &vec){
        assert(vec.rows() == 1);
        Eigen::MatrixXd bias(1,1);
        bias(0,0) = 1;
        Eigen::MatrixXd newVec(1, vec.cols()+1);
        newVec << vec, bias;
        return newVec;
    }
    
}

class MultilayerNeuralNetwork : public NeuralNetwork{

public://make this private later
    
    inline double distanceFrom(const MultilayerNeuralNetwork &nn) const {
        Eigen::MatrixXd diff = _vecForm() - nn._vecForm();
        assert(diff.cols() > 1);
        assert(diff.rows() == 1);
        const Eigen::MatrixXd product = diff*(diff.transpose());
        assert(product.rows()*product.cols() == 1);
        return sqrt(product(0,0));
    }
    
    inline Eigen::MatrixXd _vecForm() const{
        Eigen::MatrixXd output(1,0);
        for (int i=0; i<weights.size(); i++){
            long dim = weights[i].rows() * weights[i].cols();
            Eigen::MatrixXd tmp(1, output.cols()+dim);
            Eigen::MatrixXd weightCopy = weights[i];
            weightCopy.resize(1, dim);
            tmp << output, weightCopy;
            output = tmp;
        }
        return output;
    }
    
    std::vector<Eigen::MatrixXd> weights;
    std::vector<std::vector<glm::dvec3>> layers;
    
    std::vector<glm::dvec3> springPosVec;
    glm::dvec3 springStartingPos;
    int hiddenDimension;
    
public:
    
    MultilayerNeuralNetwork(std::vector<glm::dvec3> springPos, glm::dvec3 startingPos, int numHidden, int dimHidden);
    
    MultilayerNeuralNetwork(){}
    
    MultilayerNeuralNetwork (const MultilayerNeuralNetwork &old_nn){
        *this = old_nn;
    }
    
    MultilayerNeuralNetwork& operator=(const MultilayerNeuralNetwork& old_nn);
    
    void calculateNeuronPositions();
    
    virtual void evaluate(std::vector<Spring *> &springs) const;
    
    MultilayerNeuralNetwork crossOver(MultilayerNeuralNetwork &nn);
    
    MultilayerNeuralNetwork &mutate();
    
    friend std::ostream &operator<<(std::ostream &os, MultilayerNeuralNetwork &nn);
    
    void writeTo(const char *filePath);
    
    MultilayerNeuralNetwork(std::vector<double> vec);
    
};

#endif /* neural_network_h */
