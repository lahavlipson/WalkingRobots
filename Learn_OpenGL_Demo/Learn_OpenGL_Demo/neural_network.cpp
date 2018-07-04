//
//  neural_network.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "neural_network.h"


NeuralNetwork::NeuralNetwork(std::vector<glm::vec3> springPos, glm::vec3 startingPos, int numHidden, int dimHidden){
    hiddenDimension = dimHidden;
    springPosVec = springPos;
    const int numLayers = 2 + numHidden;
    const int numWeights = 1 + numHidden;
    weights = std::vector<Eigen::MatrixXd>(numWeights);
    layers = std::vector<std::vector<glm::vec3>>(numLayers);
    
    //std::cout << "weights.size(): " << weights.size() << std::endl;
    for (int i=0; i<weights.size(); i++){
        int previousSize = dimHidden;
        int nextSize = dimHidden;
        if (i == 0)
            previousSize = springPosVec.size();
        if (i == weights.size()-1)
            nextSize = springPosVec.size();
        assert(nextSize < 1000 && previousSize < 1000);
        weights[i] = (Eigen::MatrixXd::Random(previousSize,nextSize))*1.0f;
       // std::cout << weights[0].cols() << " " << i << std::endl;
    }
    
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            layers[i] = springPosVec;
        } else {
            std::vector<glm::vec3> layer;
            for (int i=0; i<dimHidden; i++)
                layer.push_back(startingPos);
            layers[i] = layer;
        }
    }
    
    //calculateNeuronPositions();
}

NeuralNetwork& NeuralNetwork::operator=(const NeuralNetwork& old_nn){
    springPosVec = old_nn.springPosVec;
    springStartingPos = old_nn.springStartingPos;
    weights = old_nn.weights;
    hiddenDimension = old_nn.hiddenDimension;
    
    const int numLayers = old_nn.layers.size();
    layers = std::vector<std::vector<glm::vec3>>(numLayers);
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            layers[i] = springPosVec;
        } else {
            std::vector<glm::vec3> layer;
            for (int i=0; i<hiddenDimension; i++)
                layer.push_back(springStartingPos);
            layers[i] = layer;
        }
    }
    
    return *this;
}

void NeuralNetwork::calculateNeuronPositions(){
    //Setting positions for layers:
    
    
    for (int k=0; k<10; k++){//arbitrary number of cycles
        for (int r=1; r<layers.size()-1; r++){//for each hidden layer
            assert(layers.size() > 2);
            for (int i=0; i<layers[1].size(); i++){//nodes in current row
                glm::vec3 average;
                float totalWeight = 0.0f;
                //next nodes
                for (int j=0; j<layers[r+1].size(); j++){
                    assert(layers[r+1][j][0] < 100);
                    const float singleWeight = fabsf(float((weights[r])(i,j)));
                    average += layers[r+1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                
                //previous nodes
                for (int j=0; j<layers[r-1].size(); j++){
                    assert(layers[r-1][j][0] < 100);
                    const float singleWeight = fabsf(float((weights[r-1])(j,i)));
                    average += layers[r-1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                assert(!std::isnan(average[0]));
                average *= 1.0f/totalWeight;
                assert(!std::isnan(average[0]));
                layers[r][i] = average;
            }
        }
    }
    
    
    
    
    //PRINTING NODE LOCS
   /* for (int i=0; i<layers.size(); i++){//layers
        for (int j=0; j<layers[i].size(); j++)//nodes
            std::cout<<"("<<layers[i][j][0]<<","<<layers[i][j][1]<<","<<layers[i][j][2]<<")  ";
        std::cout << "\n---------------------------------------------\n";
    }*/

    
}

void NeuralNetwork::evaluate(std::vector<Spring *> input){
    assert(weights.size() > 0);
    ASSERT(input.size() == weights[0].rows(), "input size was " << input.size() << ", weights[0].rows() was " << weights[0].rows() << "\n");
    Eigen::MatrixXd data(1,input.size());
    for (int i=0; i<input.size(); i++)
        data(0,i) = input[i]->calcLength();
    
    assert(weights.size()>0);
    for (int i=0; i<weights.size(); i++){
        assert(data.cols() == weights[i].rows());
        
        data *= weights[i];
        //std::cout << "before actived: " << data << "\n\n";
        if (i < weights.size()-1)
            nn_helper::activate(data);
        else
            nn_helper::activate(data,0.9,0.2,5.4f);
        
        //std::cout << "activated: " << data << "\n\n";
    }
    
   // PRINT(data);
    
    
    for (int i=0; i<input.size(); i++){
        input[i]->l_0 = fminf((input[i]->l_0)*data(0,i),2.5f);
    }
}

NeuralNetwork NeuralNetwork::crossOver(NeuralNetwork &nn){
    assert(layers.size()>2);
    NeuralNetwork output = *this;
    
    const long i = helper::myrand(output.weights.size());
    //std::cout << "THIS IS I: " << i << std::endl;
    assert(output.weights[i].rows() >= 5 && output.weights[i].cols()>=5);
    const long r1 = (helper::myrand(output.weights[i].rows()-4))+1;
    assert((output.weights[i].rows()-r1-3)>0);
    const long r2 = (helper::myrand(output.weights[i].rows()-r1-3))+r1+2;
    assert(output.weights[i].cols()>0);
    const long c1 = (helper::myrand(output.weights[i].cols()-4))+1;
    const long c2 = (helper::myrand(output.weights[i].cols()-c1-3))+c1+2;
    
    assert(r1>0);
    ASSERT(r2<output.weights[i].rows()-1,"output.weights[i].rows() = " << (output.weights[i].rows()));
    assert(r2-r1 > 1);
    
    const long r_section = helper::myrand(3);
    const long c_section = helper::myrand(3);
    long r_start = 0;
    long r_end = r1;
    long c_start = 0;
    long c_end = c1;
    
    if (r_section == 1){
        r_start = r1;
        r_end = r2;
    } else if (r_section == 2){
        r_start = r2;
        r_end = output.weights[i].rows();
    }
    
    if (c_section == 1){
        c_start = c1;
        c_end = c2;
    } else if (c_section == 2){
        c_start = c2;
        c_end = output.weights[i].cols();
    }
    
   // std::cout << "r_start: " << r_start << std::endl;
   // std::cout << "c_start: " << c_start << std::endl;
    
    output.weights[i].block(r_start,c_start,r_end-r_start,c_end-c_start) = nn.weights[i].block(r_start,c_start,r_end-r_start,c_end-c_start);
    
    return output;
}

NeuralNetwork &NeuralNetwork::mutate(){
    
    const int i = helper::myrand(weights.size());
    const int r = helper::myrand(weights[i].rows());
    const int c = helper::myrand(weights[i].cols());
    
  //  std::cout << i << std::endl;
  //  std::cout << r << std::endl;
  //  std::cout << c << std::endl;
    
    glm::vec3 p2 = layers[i+1][c];
    glm::vec3 p1 = layers[i][r];
    const double distance = glm::distance(p2,p1);
    
    const float height = 1.5f;
    const float width = 0.5f;
    const double stdDev = height*exp(pow(distance,2)/(-width));
    float number = helper::drawNormal(0.0,stdDev);
    weights[i](r,c) *= number;
    
    return *this;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn){
    for (int i=0; i<nn.weights.size(); i++)
        os << (nn.weights[i]).transpose() << "\n\n";
    return os;
}
