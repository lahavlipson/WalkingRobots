//
//  vec_writer.h
//  MultithreadDemo
//
//  Created by Lahav Lipson on 7/1/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef vec_writer_h
#define vec_writer_h

#include <vector>
#include <string>
#include <fstream>
#include <tuple>
#include <array>

class DotWriter {
    
    const char *systemRoot = NULL;
    
    std::vector<std::tuple<int, double>> tuples;
    
public:
    
    DotWriter(const char *root):systemRoot(root){}
    
    inline void appendData(int x, double y){
        tuples.push_back(std::make_tuple(x, y));
    }
    
    inline void writeTo(const char *filePath){
        std::ofstream myfile;
        myfile.open (std::string(systemRoot)+"/"+std::string(filePath));
        for (auto tup : tuples)
            myfile << std::get<0>(tup) << "," << std::get<1>(tup) << "\n";
        myfile.close();
    }
};

class VecWriter {

private:
    
    const char *systemRoot = NULL;
    
    std::vector<double> Y;
    
public:
    
    VecWriter(const char *root):systemRoot(root){}
    
    inline void appendData(double y, int n){
        for (int i=0; i<n; i++)
            this->Y.push_back(y);
    }
    
    inline void writeTo(const char *filePath){
        std::ofstream myfile;
        myfile.open (std::string(systemRoot)+"/"+std::string(filePath));
        for (int i=0; i<Y.size(); i++)
            myfile << (i+1) << "," << Y[i] << "\n";
        myfile.close();
    }
    
};

#endif /* vec_writer_h */
