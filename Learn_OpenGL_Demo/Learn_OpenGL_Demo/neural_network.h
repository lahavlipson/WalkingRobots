//
//  neural_network.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef neural_network_h
#define neural_network_h

#include <stdio.h>
#include "Eigen/Dense"
#include <iostream>
#include <vector>
#include <math.h>
#include "robot.h"
//#include <random>
#include <cmath>

namespace nn_helper {
    
    inline Eigen::MatrixXd &activate(Eigen::MatrixXd &vec) {
        assert(vec.rows() == 1);
        for (int i=0; i<vec.cols(); i++)
            vec(0,i) = 1/(1+exp(-1*vec(0,i)));
        return vec;
    }
    
}

class NeuralNetwork {

private:
    
    std::vector<Eigen::MatrixXd> weights;
    std::vector<std::vector<glm::vec3>> layers;
    
    Robot *rob_ptr = NULL;
    
public:
    
    NeuralNetwork(Robot &rob, int numHidden, int dimHidden);
    
    NeuralNetwork (const NeuralNetwork &old_nn){
        *this = old_nn;
    }
    
    NeuralNetwork& operator=(const NeuralNetwork& old_nn);
    
    void calculateNeuronPositions();
    
    std::vector<float> evaluate(std::vector<float> input);
    
    NeuralNetwork crossOver(NeuralNetwork &nn);
    
    NeuralNetwork &mutate(double a);
    
    friend std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn);
    
};

#endif /* neural_network_h */
