//
//  individual.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/26/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "individual.h"

Individual::Individual(IndividualType type):network(new UnstructuredNeuralNetwork(40)),age(1){
    if (type != Inactive) speed = network->calcSpeed();
}

Individual Individual::crossover(Individual ind, bool withMutate){
    UnstructuredNeuralNetwork *newNetwork = new UnstructuredNeuralNetwork(network->crossover(*(ind.network)));
    if (withMutate) newNetwork->mutate();
    return Individual(newNetwork, newNetwork->calcSpeed(), 1 + std::max(age,ind.age));
    }

void Individual::mutate(){
    network->mutate();
    speed = network->calcSpeed();
    age++;
}

bool Individual::dominates(Individual ind) const{
    return age <= ind.age && speed > ind.speed;
}

//bool operator< (Individual const& ind_a, Individual const& ind_b) {
//    if (ind_a.rank < ind_b.rank)
//        return true;
//    return ind_a.rank == ind_b.rank && ind_a.crowdingDistance > ind_b.crowdingDistance;
//}

bool Individual::operator < (Individual const& ind) const {
    return (rank < ind.rank) ||
    (rank == ind.rank && crowdingDistance > ind.crowdingDistance) ||
    (rank == ind.rank && crowdingDistance == ind.crowdingDistance && speed > ind.speed);
}


