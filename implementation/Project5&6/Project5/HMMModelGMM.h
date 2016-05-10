//
//  HMMModelGMM.hpp
//  speech3
//
//  Created by Dajian on 15/10/22.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef HMMModelGMM_hpp
#define HMMModelGMM_hpp

#include <stdio.h>
#include <string>
#include "HMMModel.h"

class HMMModelGMM: public HMMModel{
    
private:
    
    int K; // K states
    
    int FEATURE_LENGTH; // # of MFCC features
    
    int KERNEL_NUMBER; // # of GMM kernel
    
    std::vector<float> * entry_cost; // entry costs
    
    std::vector<std::vector<float>> * transition_cost; // transition costs
    
    std::vector<std::vector<MFCC_Feature>> * miu; // centers: K * KERNEL_NUMBER * FEATURE_LENGTH Matrix
    
    std::vector<std::vector<std::vector<float>>> * cov; // covariances: K * KERNEL_NUMBER * FEATURE_LENGTH Matrix
    
    std::vector<std::vector<float>> * alpha; // P(y | theta)
    
    // compute the probability of a frame xi s.t a given Gaussian distribution difined by miul and covl
    double LogGaussianProbability(const MFCC_Feature & xi, MFCC_Feature & miul, std::vector<float> & covl);
    
public:
    
    HMMModelGMM();
    
    ~HMMModelGMM();
    
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

#endif /* HMMModelGMM_hpp */
