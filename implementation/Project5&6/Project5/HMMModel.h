//
//  HMMModel.hpp
//  speech3
//
//  Created by Dajian on 15/10/14.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef HMMModel_hpp
#define HMMModel_hpp

#ifndef MFCC_Feature
#define MFCC_Feature std::vector<float>
#define MEBS 1e-100
#define PI 3.14
#endif


#include <stdio.h>
#include <vector>
#include <string>

class HMMModel{
    
public:
    
    virtual ~HMMModel(){};
    
    // get # of states
    virtual int getNumberOfStates() = 0;
    
    // transition cost from state s1 to state s2
    virtual float getCost(int s1, int s2) = 0;
    
    // Gaussian probability distance function
    virtual float distanceMFCC(const std::vector<float> & v1, int state) = 0;
    
    // import model
    virtual bool import(std::string path) = 0;

};

#endif /* HMMModel_hpp */
