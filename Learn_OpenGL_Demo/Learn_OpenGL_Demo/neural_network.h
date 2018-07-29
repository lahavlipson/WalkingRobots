//
//  neural_network.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 7/22/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef neural_network_h
#define neural_network_h

class NeuralNetwork {
    
public:
    
    virtual void evaluate(std::vector<Spring *> &springs) const{
        std::cerr << "This shouldn't be called!\n";
        assert(false);
    }
    
};

#endif /* neural_network_h */
