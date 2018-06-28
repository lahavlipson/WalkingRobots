//
//  helper.h
//  Learn_OpenGL_Demo
//
//  Created by Lahav Lipson on 6/17/18.
//  Copyright © 2018 Lahav Lipson. All rights reserved.
//

#ifndef helper_h
#define helper_h

#include "mass.h"
#include "spring.h"
#include "robot.h"
#include <tuple>
#include "omp.h"
#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace helper {
        inline std::vector<std::string> split(std::string s, std::string delimiter){
            std::vector<std::string> vec;
    
            size_t pos = 0;
            std::string token;
            while ((pos = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, pos);
                vec.push_back(token);
                s.erase(0, pos + delimiter.length());
            }
            vec.push_back(s);
    
            return vec;
        }
    
    inline Mass *getCenterVec(Mass *m1, Mass *m2, Mass *m3){
        const glm::vec3 scaledNormalVec = glm::cross((m2->pos)-(m1->pos),(m3->pos)-(m1->pos));
        const glm::vec3 normalVec = glm::normalize(scaledNormalVec)*1.5f;
        const glm::vec3 centroid = ((m1->pos)+(m2->pos)+(m3->pos))*0.333f;
        return new Mass(centroid + normalVec, MASS_WEIGHT);

    }
    
    inline std::string vecToStr(std::vector<float> vec){
        std::string output = "";
        for (int i=0; i<vec.size(); i++) {
            if (i > 0)
                output += ", ";
            output += std::to_string(vec[i]);
        }
        return output;
    }
    
}

#endif /* helper_h */
