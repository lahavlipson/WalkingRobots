//
//  learn.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/27/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include <stdio.h>
#include "learn.h"

NeuralNetwork learn::evolveNeuralNetwork(int generations){
    const int POPULATION_SIZE = 64;
    Robot rob = starting_models::getTetroid();
    NeuralNetwork nn(rob, 4, 10);
    
//    std::vector<std::tuple<NeuralNetwork,float> *> population;
//    for (int i=0; i<POPULATION_SIZE; i++){
//        std::tuple<NeuralNetwork,float> *tup_pointer = new std::tuple<NeuralNetwork,float>(nn,)
//    }
    
    
    return nn;
}
