//
//  HMMModelSGM.hpp
//  speech3
//
//  Created by Dajian on 15/10/22.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef HMMModelSGM_hpp
#define HMMModelSGM_hpp

#include <stdio.h>
#include <string>
#include "HMMModel.h"

class HMMModelSGM: public HMMModel{

private:
    
    int K; // K states
    
    int FEATURE_LENGTH; // # of MFCC features
    
    std::vector<float> * entry_cost; // entry costs
    
    std::vector<std::vector<float>> * transition_cost; // transition costs
    
    std::vector<MFCC_Feature> * miu; // centers: K by FEATURE_LENGTH Matrix
    
    std::vector<std::vector<float>> * cov; // covariances: K by FEATURE_LENGTH Matrix

public:
    
    HMMModelSGM();
    
    ~HMMModelSGM();
    
    void clear();
    
    // get # of states
    int getNumberOfStates();
    
    // transition cost from state s1 to state s2
    float getCost(int s1, int s2);
    
    // Gaussian probability distance function
    float distanceMFCC(const std::vector<float> & v1, int state);
    
    // import model
    bool import(std::string path);
    
};

#endif /* HMMModelSGM_hpp */
