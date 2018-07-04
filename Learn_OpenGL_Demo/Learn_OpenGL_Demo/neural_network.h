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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Eigen/Dense"
#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <cmath>
#include "spring.h"
#include "helper.h"

namespace nn_helper {
    
    inline Eigen::MatrixXd &activate(Eigen::MatrixXd &vec, float lift = 0.0f, float height = 1.0f, float width = 1.0f) {
        assert(vec.rows() == 1);
        for (int i=0; i<vec.cols(); i++)
            vec(0,i) = height/(1.0f+exp(-1.0f*(vec(0,i)/width))) + lift;
        return vec;
    }
    
}

class NeuralNetwork {

public://make this private later
    
    std::vector<Eigen::MatrixXd> weights;
    std::vector<std::vector<glm::vec3>> layers;
    
    std::vector<glm::vec3> springPosVec;
    glm::vec3 springStartingPos;
    int hiddenDimension;
    
public:
    
    NeuralNetwork(std::vector<glm::vec3> springPos, glm::vec3 startingPos, int numHidden, int dimHidden);
    
    NeuralNetwork (const NeuralNetwork &old_nn){
        *this = old_nn;
    }
    
    NeuralNetwork& operator=(const NeuralNetwork& old_nn);
    
    void calculateNeuronPositions();
    
    void evaluate(std::vector<Spring *> input);
    
    NeuralNetwork crossOver(NeuralNetwork &nn);
    
    NeuralNetwork &mutate();
    
    friend std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn);
    
};

#endif /* neural_network_h */
