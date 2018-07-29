//
//  unstructured_neural_network.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/20/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "unstructured_neural_network.h"

UnstructuredNeuralNetwork UnstructuredNeuralNetwork::crossover(UnstructuredNeuralNetwork &nn){
    UnstructuredNeuralNetwork output = nn;
    
    const long size = weights.rows();
    const long r1 = (helper::myrand(size-4))+1;
    const long r2 = (helper::myrand(size-r1-3))+r1+2;
    const long c1 = (helper::myrand(size-4))+1;
    const long c2 = (helper::myrand(size-c1-3))+c1+2;
    
    assert(r1>0);
    assert(r2-r1 > 1);
    assert(r2<size-1);
    
    output.weights.block(r1,0,r2-r1,c1) = weights.block(r1,0,r2-r1,c1);//Left
    output.weights.block(0,c1,r1,c2-c1) = weights.block(0,c1,r1,c2-c1) ;//Top
    output.weights.block(r2,c1,size-r2,c2-c1) = weights.block(r2,c1,size-r2,c2-c1);//Bottom
    output.weights.block(r1,c2,r2-r1,size-c2) = weights.block(r1,c2,r2-r1,size-c2);//Right
    
    return output;
}

UnstructuredNeuralNetwork &UnstructuredNeuralNetwork::mutate(){
    const long size = weights.rows();
    const int r = helper::myrand(size);
    const int c = helper::myrand(size);
    
    const double stdDev = 1;
    double number = helper::drawNormal(0.0,stdDev);
    const double WEIGHT_SCALE = 1.0;
    weights(r,c) += number*WEIGHT_SCALE;
    
    return *this;
}

void UnstructuredNeuralNetwork::evaluate(Eigen::MatrixXd &layer, int num_propagate) const{
    for (int n=0; n<num_propagate; n++){
        layer = weights*layer;
        if (n < num_propagate-1)
            nn_helper::activate(layer,0,1.0,1.0);
    }
    nn_helper::activate(layer,0.96,0.08,1.0);
}

void UnstructuredNeuralNetwork::evaluate(std::vector<Spring *> &springs) const{
    const long size = weights.rows();
    Eigen::MatrixXd layer = Eigen::MatrixXd::Constant(size,1,0);
    for (int i=0; i<springs.size(); i++)
        layer(i,0) = springs[i]->calcLength();
    
    evaluate(layer, 2);
    
    for (int i=0; i<springs.size(); i++)
        springs[i]->l_0 = helper::restr((springs[i]->l_0)*layer(i,0),0.5,2.4);
    
}

double UnstructuredNeuralNetwork::calcSpeed(){
    Robot rob = starting_models::getArrow();
    rob.setNN(this);
    return rob.simulate(0,SIM_TIME);
}

double UnstructuredNeuralNetwork::distanceFrom(const UnstructuredNeuralNetwork &nn) const {
    Eigen::MatrixXd diff = _vecForm() - nn._vecForm();
    assert(diff.cols() > 1);
    assert(diff.rows() == 1);
    const Eigen::MatrixXd product = diff*(diff.transpose());
    assert(product.rows()*product.cols() == 1);
    return sqrt(product(0,0));
}

Eigen::MatrixXd UnstructuredNeuralNetwork::_vecForm() const{
    const long size = weights.rows();
    Eigen::MatrixXd weights_copy = weights;
    weights_copy.resize(1,size*size);
    return weights_copy;
}

void UnstructuredNeuralNetwork::writeTo(const char *filePath){
    std::ofstream myfile;
    myfile.open (std::string(filePath));
    Eigen::MatrixXd weightMatrix = weights;
    Eigen::MatrixXd v_copy = weightMatrix;
    v_copy.resize(1, weightMatrix.rows()*weightMatrix.cols());  // No copy
    for (long i=0; i<v_copy.cols(); i++){
        myfile << v_copy(0,i);
        if (i < v_copy.cols()-1)
            myfile << ",";
    }
    myfile.close();
}

bool operator==(const UnstructuredNeuralNetwork& lhs, const UnstructuredNeuralNetwork& rhs) {
    return lhs.weights == rhs.weights;
}

std::ostream &operator<<(std::ostream &os, UnstructuredNeuralNetwork &nn){
    return os << nn.weights;
}
