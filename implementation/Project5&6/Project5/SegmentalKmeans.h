//
//  SegmentalKmeans.hpp
//  speech3
//
//  Created by Dajian on 15/10/9.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef SegmentalKmeans_hpp
#define SegmentalKmeans_hpp

#ifndef MFCC_Feature
#define MFCC_Feature std::vector<float>
#define MEBS 1e-100
#define PI 3.14
#endif

#include <vector>
#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>

class SegmentalKmeans{
    
private:
    
    int K; // K states
    
    int FEATURE_LENGTH; // # of MFCC features
    
    int MAX_ITERATIONS; // maximum # of iteration
    
    float MIN_DELTA; // convergence threshold
    
    std::vector<float> * entry_cost; // entry costs
    
    std::vector<std::vector<float>> * transition_cost; // transition costs
    
    std::vector<MFCC_Feature> * miu; // centers: K by FEATURE_LENGTH Matrix
    
    std::vector<std::vector<float>> * cov; // covariances: K by FEATURE_LENGTH Matrix
    
    MFCC_Feature & addBToA(MFCC_Feature & A, const MFCC_Feature & B); // to add vector B to vector A
    
    MFCC_Feature & divideAByB(MFCC_Feature & A, long B); // to divide vector A by B
    
    void estimateCovariances(std::vector<std::vector<MFCC_Feature>> & container); // to estimate covariance
    
public:
    
    // Gaussian probability distance function
    float distanceMFCC(const MFCC_Feature & v1, int state);
    
    // construction
    SegmentalKmeans();
    
    // set parameters of the model
    void setParameters(int K, int FEATURE_LENGTH);
    
    // set maximum # of iteration
    void setMaxIteration(int max_iteration);
    
    // set convergence threshold
    void setMinDelta(int min_delta);
    
    // commit single Gaussian HMM
    void commit(std::vector<std::vector<MFCC_Feature>>);
    
    // destruction
    ~SegmentalKmeans();
    
    // transition cost
    float getCost(int s1, int s2);
    
    // get # of states
    int getNumberOfStates();
    
    // output the model
    void output(std::string path);
};

#endif /* SegmentalKmeans_hpp */
