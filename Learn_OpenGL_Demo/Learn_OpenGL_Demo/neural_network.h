//
//  neural_network.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/22/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef neural_network_h
#define neural_network_h

#include "Eigen/Dense"

const double SIM_TIME = 300;

class NeuralNetwork {
    
public:
    
    virtual Eigen::MatrixXd _vecForm() const {
        std::cerr << "This shouldn't be called!\n";
        abort();
        return Eigen::MatrixXd(0,0);
    }
    
    virtual void evaluate(std::vector<Spring *> &springs) const{
        std::cerr << "This shouldn't be called!\n";
        abort();
    }
    
    virtual void mutate() {
        std::cerr << "This shouldn't be called!\n";
        abort();
    }
    
    virtual double calcSpeed() {
        std::cerr << "This shouldn't be called!\n";
        abort();
        return -1;
    }
    
    virtual NeuralNetwork *crossover(NeuralNetwork *nn) const {
        std::cerr << "This shouldn't be called!\n";
        abort();
        return nullptr;
    }
    
    virtual double distanceFrom(const NeuralNetwork *nn) const {
        std::cerr << "This shouldn't be called!\n";
        abort();
        return -1;
    }
    
    virtual void writeTo(const char *filePath){
        std::cerr << "This shouldn't be called!\n";
        abort();
    }
    
    virtual ~NeuralNetwork(){}
    
};

#endif /* neural_network_h */
