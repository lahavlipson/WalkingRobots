//
//  neural_network.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "multilayer_neural_network.h"

const double WEIGHT_SCALE = 1;

MultilayerNeuralNetwork::MultilayerNeuralNetwork(int inputSize, int numHidden, int dimHidden){
    hiddenDimension = dimHidden;
    const int numWeights = 1 + numHidden;
    weights = std::vector<Eigen::MatrixXd>(numWeights);
    
    for (int i=0; i<weights.size(); i++){
        long previousSize = hiddenDimension + 1;
        long nextSize = hiddenDimension;
        if (i == 0)
            previousSize = inputSize+1;
        if (i == weights.size()-1)
            nextSize = inputSize;
        assert(nextSize < 1000 && previousSize < 1000);
        weights[i] = (Eigen::MatrixXd::Random(previousSize,nextSize))*WEIGHT_SCALE;
    }
}

MultilayerNeuralNetwork::MultilayerNeuralNetwork(std::vector<glm::dvec3> springPos, glm::dvec3 startingPos, int numHidden, int dimHidden){
    hiddenDimension = dimHidden;
    springPosVec = springPos;
    const int numLayers = 2 + numHidden;
    const int numWeights = 1 + numHidden;
    weights = std::vector<Eigen::MatrixXd>(numWeights);
    layers = std::vector<std::vector<glm::dvec3>>(numLayers);
    
    for (int i=0; i<weights.size(); i++){
        long previousSize = hiddenDimension + 1;
        long nextSize = hiddenDimension;
        if (i == 0)
            previousSize = springPosVec.size()+1;
        if (i == weights.size()-1)
            nextSize = springPosVec.size();
        assert(nextSize < 1000 && previousSize < 1000);
        weights[i] = (Eigen::MatrixXd::Random(previousSize,nextSize))*WEIGHT_SCALE;
       // std::cout << weights[0].cols() << " " << i << std::endl;
        
    }
    
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            layers[i] = springPosVec;
        } else {
            std::vector<glm::dvec3> layer;
            for (int i=0; i<hiddenDimension+1; i++)
                layer.push_back(startingPos);
            layers[i] = layer;
        }
    }
    
    //calculateNeuronPositions();
}

MultilayerNeuralNetwork& MultilayerNeuralNetwork::operator=(const MultilayerNeuralNetwork& old_nn){
    springPosVec = old_nn.springPosVec;
    springStartingPos = old_nn.springStartingPos;
    weights = old_nn.weights;
    hiddenDimension = old_nn.hiddenDimension;
    
    const int numLayers = old_nn.layers.size();
    layers = std::vector<std::vector<glm::dvec3>>(numLayers);
    for (int i=0; i<numLayers; i++){
        if (i == 0 || i == numLayers-1){
            layers[i] = springPosVec;
        } else {
            std::vector<glm::dvec3> layer;
            for (int i=0; i<hiddenDimension+1; i++)
                layer.push_back(springStartingPos);
            layers[i] = layer;
        }
    }
    
    return *this;
}

void MultilayerNeuralNetwork::calculateNeuronPositions(){
    //Setting positions for layers:
    
    return;//I just don't wanna bother rn
    
    for (int k=0; k<10; k++){//arbitrary number of cycles
        for (int r=1; r<layers.size()-1; r++){//for each hidden layer
            assert(layers.size() > 2);
            for (int i=0; i<layers[1].size(); i++){//nodes in current row
                glm::dvec3 average;
                double totalWeight = 0.0;
                //next nodes
                for (int j=0; j<layers[r+1].size(); j++){
                    assert(layers[r+1][j][0] < 100);
                    const double singleWeight = fabsf(double((weights[r])(i,j)));
                    average += layers[r+1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                
                //previous nodes
                for (int j=0; j<layers[r-1].size(); j++){
                    assert(layers[r-1][j][0] < 100);
                    const double singleWeight = fabsf(double((weights[r-1])(j,i)));
                    average += layers[r-1][j]*singleWeight;
                    totalWeight += singleWeight;
                }
                assert(!std::isnan(average[0]));
                average *= 1.0/totalWeight;
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

void MultilayerNeuralNetwork::evaluate(std::vector<Spring *> &springs) const{
    assert(weights.size() > 0);
    ASSERT(springs.size()+1 == weights[0].rows(), "input size+1 was " << (springs.size()+1) << ", weights[0].rows() was " << weights[0].rows() << "\n");
    Eigen::MatrixXd data(1,springs.size());
    for (int i=0; i<springs.size(); i++)
        data(0,i) = springs[i]->calcLength();
    data = nn_helper::appendBias(data);
    
    //Printing out check
    //std::cout << "Data: " << data << std::endl;
    
    
    assert(weights.size()>0);
    for (int i=0; i<weights.size(); i++){
        assert(data.cols() == weights[i].rows());
        
        data *= weights[i];
        //std::cout << "before actived: " << data << "\n\n";
        if (i < weights.size()-1){
            nn_helper::activate(data);
            data = nn_helper::appendBias(data);
        } else {
            nn_helper::activate(data,0.96,0.08,1.0);
        }
        
        //std::cout << "activated: " << data << "\n\n";
    }
    
    for (int i=0; i<springs.size(); i++){
        springs[i]->l_0 = helper::restr((springs[i]->l_0)*data(0,i),0.5,2.4);
    }
}

NeuralNetwork *MultilayerNeuralNetwork::crossover(NeuralNetwork *nn) const {
    //assert(layers.size()>2);
    MultilayerNeuralNetwork *output = new MultilayerNeuralNetwork(*(dynamic_cast<MultilayerNeuralNetwork *>(nn)));
    
    const long i = helper::myrand(output->weights.size());
    assert(output->weights[i].rows() >= 5 && output->weights[i].cols()>=5);
    const long r1 = (helper::myrand(output->weights[i].rows()-4))+1;
    assert((output->weights[i].rows()-r1-3)>0);
    const long r2 = (helper::myrand(output->weights[i].rows()-r1-3))+r1+2;
    assert(output->weights[i].cols()>0);
    const long c1 = (helper::myrand(output->weights[i].cols()-4))+1;
    const long c2 = (helper::myrand(output->weights[i].cols()-c1-3))+c1+2;
    
    assert(r1>0);
    ASSERT(r2<output->weights[i].rows()-1,"output.weights[i].rows() = " << (output->weights[i].rows()));
    assert(r2-r1 > 1);
    
    output->weights[i].block(r1,0,r2-r1,c1) = weights[i].block(r1,0,r2-r1,c1);//Left
    output->weights[i].block(0,c1,r1,c2-c1) = weights[i].block(0,c1,r1,c2-c1) ;//Top
    output->weights[i].block(r2,c1,weights[i].rows()-r2,c2-c1) = weights[i].block(r2,c1,weights[i].rows()-r2,c2-c1);//Bottom
    output->weights[i].block(r1,c2,r2-r1,weights[i].cols()-c2) = weights[i].block(r1,c2,r2-r1,weights[i].cols()-c2);//Right
    
    return output;
}

double MultilayerNeuralNetwork::calcSpeed() {
    Robot rob = starting_models::getArrow();
    rob.setNN(this);
    return rob.simulate(0,SIM_TIME);
}

void MultilayerNeuralNetwork::mutate(){
    
    const int i = helper::myrand(weights.size());
    const int r = helper::myrand(weights[i].rows());
    const int c = helper::myrand(weights[i].cols());
    
//    glm::dvec3 p2 = layers[i+1][c];
//    glm::dvec3 p1 = layers[i][r];
//    const double distance = glm::distance(p2,p1);
    
    const double height = 1.5;
    const double width = 0.5;
    const double stdDev = 1;//height*exp(pow(distance,2)/(-width));
    double number = helper::drawNormal(0.0,stdDev);
    weights[i](r,c) += number*WEIGHT_SCALE;
    //PRINT_F(weights[i](r,c));
    
}

std::ostream &operator<<(std::ostream &os, MultilayerNeuralNetwork &nn){
    for (int i=0; i<nn.weights.size(); i++)
        os << "Row " << i << "\n" << (nn.weights[i]).transpose() << "\n\n";
    return os;
}

void MultilayerNeuralNetwork::writeTo(const char *filePath){
    std::ofstream myfile;
    myfile.open (std::string(filePath));
    myfile << hiddenDimension << ",";
    for (int w=0; w<weights.size(); w++){
        Eigen::MatrixXd weightMatrix = weights[w];
        myfile << weightMatrix.rows() << "," << weightMatrix.cols();
        Eigen::MatrixXd v_copy = weightMatrix;
        v_copy.resize(1, weightMatrix.rows()*weightMatrix.cols());  // No copy
        for (long i=0; i<v_copy.cols(); i++)
            myfile << "," << v_copy(0,i);
        if (w < weights.size()-1)
            myfile << ",";
    }
    myfile.close();
}

double MultilayerNeuralNetwork::distanceFrom(const NeuralNetwork *nn) const {
    Eigen::MatrixXd diff = _vecForm() - nn->_vecForm();
    assert(diff.cols() > 1);
    assert(diff.rows() == 1);
    const Eigen::MatrixXd product = diff*(diff.transpose());
    assert(product.rows()*product.cols() == 1);
    return sqrt(product(0,0));
}

Eigen::MatrixXd MultilayerNeuralNetwork::_vecForm() const{
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

MultilayerNeuralNetwork::MultilayerNeuralNetwork(std::vector<double> vec){
    hiddenDimension = (int)vec[0];
    for (int i=1; i<vec.size();){
        int rows = vec[i+0];
        int cols = vec[i+1];
        Eigen::MatrixXd weightMat(1,rows*cols);
        for (int j=0; j<rows*cols; j++)
            weightMat(0,j) = vec[i+2+j];
        weightMat.resize(rows,cols);
        weights.push_back(weightMat);
        i += 2 + rows*cols;
    }  
}
