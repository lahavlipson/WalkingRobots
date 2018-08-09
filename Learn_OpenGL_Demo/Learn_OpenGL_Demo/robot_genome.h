//
//  robot_genome.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 8/5/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef robot_genome_h
#define robot_genome_h

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <float.h>
#include "helper.h"

struct GenomeBase {
    
private:
    glm::dvec3 loc;
    
public:
    
    int ID;
    std::vector<int> neighbors;
    
    bool isLateral(){
        return loc[0] > 0;
    }
    
    glm::dvec3 getLoc(){
        return loc;
    }
    
    glm::dvec3 getSibLoc(){
        return glm::dvec3(-loc[0], loc[1], loc[2]);
    }
    
    void addNeighbor(int nID){
        neighbors.push_back(nID);
    }
    
    static int generateID(){
        return helper::myrand(9000000)+1000000;
    }
    
    bool hasNeighbor(int nID){
        return std::find(neighbors.begin(), neighbors.end(), nID) != neighbors.end();
    }
    
    void removeNeighbor(int nID){
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), nID), neighbors.end());
    }
    
    GenomeBase(glm::dvec3 l, int i = generateID()):ID(i), loc(l){
        l[1] = std::abs(l[1]);
    }
    
    GenomeBase(){}
    
    friend std::ostream &operator<<(std::ostream &os, GenomeBase &base);
};

class RobotGenome {
    
private:
    
    GenomeBase *getBase(int ID){
        for (GenomeBase &gb : bases){
            if (gb.ID == ID)
                return &gb;
        }
        return nullptr;
    }
    
    void addBase(GenomeBase gb){
        bases.push_back(gb);
    }
    
    glm::dvec3 getNewLoc(bool lateral);
    
    std::vector<int> getListOfIDs(glm::dvec3 *sortByDist = nullptr);
    
public:
    
    std::vector<GenomeBase> bases;
    
    RobotGenome();
    
    RobotGenome(std::vector<double> vec);
    
    int getNumMasses();
    
    void mutate();
    
    //Should move to private
    void extend();
    void removeRandomBase();
    
    GenomeBase &operator[] (const int i){
        return bases[i];
    }
    
    friend std::ostream &operator<<(std::ostream &os, RobotGenome &nn);
    
    std::size_t size() const{
        return bases.size();
    }
    
    void writeTo(const char *filePath);
    
};

#endif /* robot_genome_h */
