//
//  starting_models.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/23/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef starting_models_h
#define starting_models_h

#include <stdio.h>
#include "mass.h"
#include "spring.h"
#include "robot.h"
#include <tuple>
#include "omp.h"
#include <algorithm>
#include <vector>
#include "helper.h"
#include "neural_network.h"

namespace starting_models {
    Robot getCuboid(int x, int y, int z);
    Robot getTetrahedron();
    Robot getCube();
    Robot getTetroid();
    Robot getArrow();
}

#endif /* starting_models_h */
