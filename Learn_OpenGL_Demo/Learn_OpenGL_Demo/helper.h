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
#include <tuple>
#include "omp.h"
#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iterator>

#define PRINT(x) std::cout << #x" is " << x << std::endl
#define PRINT_I(x) printf(#x" is %d\n",x)
#define PRINT_F(x) printf(#x" is %f\n",x)

#ifndef NDEBUG
#   define ASSERT(condition, message) \
do { \
if (! (condition)) { \
std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
<< " line " << __LINE__ << ": " << message << std::endl; \
std::terminate(); \
} \
} while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

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
    
    inline double restr(double f, double min, double max){
        assert(max>min);
        if (f < min)
            return min;
        return std::min(f,max);
    }
    
    inline float drawNormal(float mean, float stdDev){
        std::random_device rd;
        std::mt19937 e2(rd());
        std::normal_distribution<> dist(mean, stdDev);
        return dist(e2);
    }
    
    inline int myrand(int n, int exclude = -1){
        if (n==0) return 0;
        ASSERT(n>0 && n<1000,"n is " << n);
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, n-1);
        int output;
        do {
            output = dis(gen);
        } while (output == exclude);
        return output;
    }
    
    inline float myrandFloat(){
        return float(myrand(1000000))/1000000.0f;
    }
    
    inline float calcMean(std::vector<float> vec){
        float total = 0;
        for (float f : vec)
            total += f;
        return total/vec.size();
    }
    
    inline float calcVariance(std::vector<float> vec){
        const float mean = calcMean(vec);
        float var = 0;
        for (float f : vec)
            var += (f-mean)*(f-mean);
        return var/(vec.size()-1);
    }
    
}

#endif /* helper_h */
