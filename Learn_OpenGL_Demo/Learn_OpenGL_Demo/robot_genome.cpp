//
//  robot_genome.cpp
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 8/5/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#include "robot_genome.h"

std::ostream &operator<<(std::ostream &os, GenomeBase &base){
    os << "[" << base.ID << ",(" << base.loc[0] << "," << base.loc[1] << "," << base.loc[2] << ")";
    for (int i=0; i<base.neighbors.size(); i++)
        os << "," << base.neighbors[i];
    return os << "]";
}

std::ostream &operator<<(std::ostream &os, RobotGenome &genome){
    os << "{ ";
    for (int i=0; i<genome.bases.size(); i++)
        os << genome.bases[i] << " ";
    return os << "}";
}

RobotGenome::RobotGenome() {
    GenomeBase gb1(glm::dvec3(0,0,-2));
    addBase(gb1);
    
    GenomeBase gb2(glm::dvec3(1.1547,0,0));
    gb2.addNeighbor(gb1.ID);
    gb2.addNeighbor(gb2.ID);
    addBase(gb2);
    
    GenomeBase gb3(glm::dvec3(0,1,-0.7));
    gb3.addNeighbor(gb1.ID);
    gb3.addNeighbor(gb2.ID);
    addBase(gb3);
}

RobotGenome::RobotGenome(std::vector<double> vec){
    int id,numNeighbors,index = 0;
    glm::dvec3 location;
    GenomeBase gb;
    
    while (index < vec.size()){
        id = vec[index];
        location = glm::dvec3(vec[index+1],vec[index+2],vec[index+3]);
        gb = GenomeBase(location, id);
        numNeighbors = vec[index+4];
        index += 5;
        for (int n=index+numNeighbors; index<n; index++)
            gb.addNeighbor(vec[index]);
        bases.push_back(gb);
    }
}

void RobotGenome::writeTo(const char *filePath){
    std::ofstream myfile;
    myfile.open (std::string(filePath));
    for (long i=0; i<bases.size(); i++){
        myfile << bases[i].ID << "," << bases[i].getLoc()[0] << "," << bases[i].getLoc()[1] << ","<< bases[i].getLoc()[2];
        myfile << "," << bases[i].neighbors.size() ;
        for (int n=0; n<bases[i].neighbors.size(); n++){
            myfile << "," << bases[i].neighbors[n];
        }
        if (i < bases.size()-1) myfile << ",";
    }
    myfile.close();
}

int RobotGenome::getNumMasses(){
    int count = 0;
    for (GenomeBase &gb : bases)
        count += (1 + gb.isLateral());
    return count;
}

std::vector<int> RobotGenome::getListOfIDs(glm::dvec3 *sortByDist){
    std::vector<GenomeBase> basesCopy = bases;
    
    if (sortByDist) {
        int i, j, min_idx;
        // One by one move boundary of unsorted subarray
        for (i = 0; i < basesCopy.size()-1; i++)
        {
            // Find the minimum element in unsorted array
            min_idx = i;
            for (j = i+1; j < basesCopy.size(); j++){
                if (glm::distance(basesCopy[j].getLoc(),*sortByDist) < glm::distance(basesCopy[min_idx].getLoc(),*sortByDist)) min_idx = j;
            }
            // Swap the found minimum element with the first element
            GenomeBase tmp = basesCopy[min_idx];
            basesCopy[min_idx] = basesCopy[i];
            basesCopy[i] = tmp;
        }
        
        if (basesCopy.size()>3){
        assert(glm::distance(*sortByDist,basesCopy[0].getLoc()) <= glm::distance(*sortByDist,basesCopy[1].getLoc()) && glm::distance(*sortByDist,basesCopy[1].getLoc()) <= glm::distance(*sortByDist,basesCopy[2].getLoc()) && glm::distance(*sortByDist,basesCopy[2].getLoc()) <= glm::distance(*sortByDist,basesCopy[3].getLoc()));
        }
    }
    
    std::vector<int> output;
    for (int i=0; i<basesCopy.size(); i++)
        output.push_back(basesCopy[i].ID);
    
    return output;
}

glm::dvec3 RobotGenome::getNewLoc(bool lateral){
    double MAX_X,MIN_X,MAX_Y,MIN_Y,MAX_Z,MIN_Z;
    MIN_Y = 0.0;
    MIN_X = 1.0;
    MIN_Z = -1.0;
    MAX_X = MAX_Y = MAX_Z = 1.0;
    for (GenomeBase &gb : bases){
        MAX_X = std::max(MAX_X, gb.getLoc()[0]);
        MAX_Y = std::max(MAX_Y, gb.getLoc()[1]);
        MAX_Z = std::max(MAX_Z, gb.getLoc()[2]);
        MIN_Z = std::min(MIN_Z, gb.getSibLoc()[2]);
    }
    
    glm::dvec3 location;
    const double MAX_DIST = 1.4;
    const double MIN_DIST = 0.8;
    const double PADDING = 1.2;
    double dist;
    
    do {
        double x,y,z;
        
        if (lateral) x = helper::myrandrange(MIN_X,MAX_X+PADDING); else x = 0;
        y = helper::myrandrange(MIN_Y,MAX_Y+PADDING);
        z = helper::myrandrange(MIN_Z-PADDING,MAX_Z+PADDING);
        location = glm::dvec3(x,y,z);
        
        dist = DBL_MAX;
        for (GenomeBase &gb : bases){
            dist = std::min(dist, glm::distance(gb.getLoc(), location));
            dist = std::min(dist, glm::distance(gb.getSibLoc(), location));
        }
    } while ((dist < MIN_DIST || dist > MAX_DIST) && bases.size() > 0);
    
    return location;
}

void RobotGenome::removeRandomBase(){
    //Remove old one
    const int indexToRemove = helper::myrand(bases.size());
    const int idToRemove = bases[indexToRemove].ID;
    
    for (GenomeBase &gb : bases){
        if (gb.hasNeighbor(idToRemove)){
            gb.removeNeighbor(idToRemove);
            
            int closestID = -1;
            double minDist = DBL_MAX;
            for (int nID : gb.neighbors){
                if (glm::distance(getBase(nID)->getLoc(),gb.getLoc()) < minDist){
                    minDist = glm::distance(getBase(nID)->getLoc(),gb.getLoc());
                    closestID = nID;
                }
            }
            gb.addNeighbor(closestID);
        }

    }
    
    
    bases.erase(bases.begin() + indexToRemove);
    
}

void RobotGenome::extend(){
    const bool isLateral = helper::myrand(2);
    glm::dvec3 location = getNewLoc(isLateral);
    assert(!isLateral || location[0] > 0);
    std::vector<int> IDs = getListOfIDs(&location);
    const int newID = GenomeBase::generateID();
    
    GenomeBase newBase(location, newID);
    newBase.addNeighbor(IDs[0]);
    newBase.addNeighbor(IDs[1]);
    newBase.addNeighbor(IDs[2]);
    
    if (isLateral && helper::myranddouble() < (1.0/(IDs.size()+1)))
        newBase.addNeighbor(newID);
    
    bases.push_back(newBase);
}

void RobotGenome::mutate(){
    if (helper::myrand(2) && getNumMasses() >= 4)
        removeRandomBase();
        
    if (helper::myrand(2))
        extend();
}
