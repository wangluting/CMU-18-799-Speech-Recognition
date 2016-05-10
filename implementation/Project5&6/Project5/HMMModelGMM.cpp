//
//  HMMModelGMM.cpp
//  speech3
//
//  Created by Dajian on 15/10/22.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#include "HMMModelGMM.h"
#include <math.h>
#include <fstream>

HMMModelGMM::HMMModelGMM(){
    K = 1;
    FEATURE_LENGTH = 1;
    KERNEL_NUMBER = 1;
    miu = NULL;
    cov = NULL;
    transition_cost = NULL;
    entry_cost = NULL;
    alpha = NULL;
}

HMMModelGMM::~HMMModelGMM(){
    clear();
}

void HMMModelGMM::clear(){
    if (miu) {
        delete miu;
        miu = NULL;
    }
    if (cov) {
        delete cov;
        cov = NULL;
    }
    if (entry_cost) {
        delete entry_cost;
        entry_cost = NULL;
    }
    if (transition_cost) {
        delete transition_cost;
        transition_cost = NULL;
    }
    if (alpha) {
        delete alpha;
        alpha = NULL;
    }
}



float HMMModelGMM::getCost(int s1, int s2){
    if (s1==0) {
        return (*entry_cost)[s2-1];
    }else{
        return (*transition_cost)[s1-1][s2-1];
    }
}

// get # of states
int HMMModelGMM::getNumberOfStates(){
    return K;
}

// P(xi | thetaj)
double HMMModelGMM::LogGaussianProbability(const MFCC_Feature & xi, MFCC_Feature & miul, std::vector<float> & covl){
    double logp = 0;
    int i;
    for (i = 0; i < FEATURE_LENGTH; i++) {
        logp = logp - 0.5 * log(2 * PI * covl[i]) - 0.5 * pow(xi[i]-miul[i],2) / covl[i];
    }
    return logp;
}

float HMMModelGMM::distanceMFCC(const MFCC_Feature & v1, int state){
    // state range: 1-
    std::vector<MFCC_Feature> & vmiu = (*miu)[state-1];
    std::vector<std::vector<float>> & vcov = (*cov)[state-1];
    std::vector<float> & valpha = (*alpha)[state-1];
    float temp_float,temp_sum = logf(valpha[0]) + LogGaussianProbability(v1, vmiu[0], vcov[0]);
    for (int i = 1; i < KERNEL_NUMBER; i++) {
        temp_float = logf(valpha[i]) + LogGaussianProbability(v1, vmiu[i], vcov[i]);
        if (temp_float - temp_sum > 10) {
            temp_sum = temp_float;
        }else{
            temp_sum += log(1 + expf(temp_float - temp_sum));
        }
    }
    return -temp_sum;
}

bool HMMModelGMM::import(std::string path){
    std::ifstream ifs(path);
    
    std::string label, stringvalue;
    
    float floatvalue;
    
    int i, j, k;
    
    while (ifs>>label) {
        if (label.compare("[TYPE]") == 0) {
            ifs>>stringvalue;
            if (stringvalue.compare("GMM") != 0) {
                printf("%s is not a Single Gaussian Model!", path.c_str());
                return false;
            }
        }else if (label.compare("[K]") == 0){
            ifs>>K;
        }else if (label.compare("[FEATURE_LENGTH]") == 0){
            ifs>>FEATURE_LENGTH;
        }else if (label.compare("[KERNEL_NUMBER]") == 0){
            ifs>>KERNEL_NUMBER;
        }else if (label.compare("[ENTRY_COST]") == 0){
            entry_cost = new std::vector<float>();
            for (i = 0; i < K; i++) {
                ifs>>floatvalue;
                entry_cost->push_back(floatvalue);
            }
        }else if (label.compare("[TRANSITION_COST]") == 0){
            transition_cost = new std::vector<std::vector<float>>();
            for (i = 0; i < K; i++) {
                MFCC_Feature * temp = new std::vector<float>();
                for (j = 0; j < K; j++) {
                    ifs>>floatvalue;
                    temp->push_back(floatvalue);
                }
                transition_cost->push_back(*temp);
            }
        }else if (label.compare("[MIU]") == 0){
            miu = new std::vector<std::vector<MFCC_Feature>>();
            for (i = 0; i < K; i++) {
                std::vector<MFCC_Feature> * temp1 = new std::vector<MFCC_Feature>();
                for (j = 0; j < KERNEL_NUMBER; j++) {
                    MFCC_Feature * temp2 = new MFCC_Feature();
                    for (k = 0; k < FEATURE_LENGTH; k++) {
                        ifs>>floatvalue;
                        temp2->push_back(floatvalue);
                    }
                    temp1->push_back(*temp2);
                }
                miu->push_back(*temp1);
            }
        }else if (label.compare("[COV]") == 0){
            cov = new std::vector<std::vector<MFCC_Feature>>();
            for (i = 0; i < K; i++) {
                std::vector<MFCC_Feature> * temp1 = new std::vector<MFCC_Feature>();
                for (j = 0; j < KERNEL_NUMBER; j++) {
                    MFCC_Feature * temp2 = new MFCC_Feature();
                    for (k = 0; k < FEATURE_LENGTH; k++) {
                        ifs>>floatvalue;
                        temp2->push_back(floatvalue);
                    }
                    temp1->push_back(*temp2);
                }
                cov->push_back(*temp1);
            }
        }else if (label.compare("[ALPHA]") == 0){
            alpha = new std::vector<std::vector<float>>();
            for (i = 0; i < K; i++) {
                std::vector<float> * temp = new std::vector<float>();
                for (j = 0; j < KERNEL_NUMBER; j++) {
                    ifs>>floatvalue;
                    temp->push_back(floatvalue);
                }
                alpha->push_back(*temp);
            }
        }
    }
    
    ifs.close();
    
    if (cov && miu && transition_cost && entry_cost && alpha) {
        return true;
    }else{
        clear();
        printf("%s is not a complete Single Gaussian Model file!", path.c_str());
        return false;
    }
}