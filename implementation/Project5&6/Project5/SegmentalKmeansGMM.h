//
//  SegmentalKmeansGMM.hpp
//  speech3
//
//  Created by Dajian on 15/10/9.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef SegmentalKmeansGMM_hpp
#define SegmentalKmeansGMM_hpp

#ifndef MFCC_Feature
#define MFCC_Feature std::vector<float>
#define MEBS 1e-100
#define MCOV 1
#define PI 3.14
#define MAX_IN_DTW 1E9
#endif

#include <vector>
#include <stdio.h>
#include <math.h>
#include <fstream>

class SegmentalKmeansGMM{
    
private:
    
    int K; // K clusters
    
    int FEATURE_LENGTH; // # of MFCC features
    
    int KERNEL_NUMBER; // # of GMM kernel
    
    int MAX_ITERATIONS; // maximum # of iteration
    
    float MIN_DELTA; // convergence threshold
    
    std::vector<float> * entry_cost; // entry costs
    
    std::vector<std::vector<float>> * transition_cost; // transition costs
    
    std::vector<std::vector<MFCC_Feature>> * miu; // centers: K * KERNEL_NUMBER * FEATURE_LENGTH Matrix
    
    std::vector<std::vector<std::vector<float>>> * cov; // covariances: K * KERNEL_NUMBER * FEATURE_LENGTH Matrix
    
    std::vector<std::vector<float>> * alpha; // P(y | theta)
    
    float EuclideanDistance(const MFCC_Feature & A, const MFCC_Feature & B);
    
    MFCC_Feature & addBToA(MFCC_Feature & A, const MFCC_Feature & B); // to add vector B to vector A
    
    MFCC_Feature & divideAByB(MFCC_Feature & A, long B); // to divide vector A by B
    
    // compute the probability of a frame xi s.t a given Gaussian distribution difined by miul and covl
    double LogGaussianProbability(const MFCC_Feature & xi, MFCC_Feature & miul, std::vector<float> & covl);
    
    // use EM to estimate GMM model, return the maximum Loss
    void estimateGMM(std::vector<std::vector<MFCC_Feature>> & container);
    
    // initialize miu and cov (Kmeans)
    void initializeGMMParameters(std::vector<std::vector<MFCC_Feature>> & container);
    
    void clear();
    
public:
    
    // distance function - GMM distance
    float distanceMFCC(const MFCC_Feature & v1, int state);
    
    // construction
    SegmentalKmeansGMM();
    

    // set parameters for HMM
    void setParameters(int K, int FEATURE_LENGTH, int KERNEL_NUMBER);
    
    // set maximum # of iteration
    void setMaxIteration(int max_iteration);
    
    // set convergence threshold
    void setMinDelta(int min_delta);
    
    // train HMM model
    void commit(std::vector<std::vector<MFCC_Feature>>);
    
    // destruction
    ~SegmentalKmeansGMM();
    
    // transition cost
    float getCost(int s1, int s2);
    
    // get # of states
    int getNumberOfStates();
    
    // output the model
    void output(std::string path);
    
};

#endif /* SegmentalKmeansGMM_hpp */
