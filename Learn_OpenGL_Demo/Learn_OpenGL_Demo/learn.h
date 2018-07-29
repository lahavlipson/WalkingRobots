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
#include "multilayer_neural_network.h"
#include "unstructured_neural_network.h"
#include "starting_models.h"
#include "vec_writer.h"
#include "individual.h"
#include <array>
#include <string>
#include <algorithm>

#define enable_graphics

namespace learn {
    
    Robot learnNeuralNetworkPareto(int generations);
    
    void nonDominatedSort(std::vector<Individual> &population);
    
}


#endif /* learn_h */
