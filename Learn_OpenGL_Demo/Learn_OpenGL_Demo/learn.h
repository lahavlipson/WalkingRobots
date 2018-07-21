//
//  learn.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/12/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef learn_h
#define learn_h
#include "mass.h"
#include "spring.h"
#include "robot.h"
#include <tuple>
#include "omp.h"
#include <algorithm>
#include <vector>
#include "helper.h"
#include "neural_network.h"
#include "starting_models.h"
#include "vec_writer.h"
#include <array>
#include <string>

#define enable_graphics

namespace learn {
    
    void checkFeedback(NeuralNetwork &adam_nn, int numGens, bool evolve);
    
    Robot learnNeuralNetwork(int generations, bool evolve);
    
    Robot learnNeuralNetworkPareto(int generations);
    
}


#endif /* learn_h */
