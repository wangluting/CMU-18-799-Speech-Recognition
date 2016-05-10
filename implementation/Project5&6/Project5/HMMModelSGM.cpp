//
//  HMMModelSGM.cpp
//  speech3
//
//  Created by Dajian on 15/10/22.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#include "HMMModelSGM.h"
#include <math.h>
#include <fstream>

HMMModelSGM::HMMModelSGM(){
    K = 1;
    FEATURE_LENGTH = 1;
    miu = NULL;
    cov = NULL;
    transition_cost = NULL;
    entry_cost = NULL;
}

HMMModelSGM::~HMMModelSGM(){
    clear();
}

void HMMModelSGM::clear(){
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
}

float HMMModelSGM::getCost(int s1, int s2){
    if (s1==0) {
        return (*entry_cost)[s2-1];
    }else{
        return (*transition_cost)[s1-1][s2-1];
    }
}

// get # of states
int HMMModelSGM::getNumberOfStates(){
    return K;
}

float HMMModelSGM::distanceMFCC(const MFCC_Feature & v1, int state){
    // state range: 1-
    float sum = 0;
    long cursor = v1.size() - 1;
    MFCC_Feature & vmiu = (*miu)[state-1];
    MFCC_Feature & vcov = (*cov)[state-1];
    while (cursor>=0) {
        sum += 0.5 * log(2 * PI * vcov[cursor]) + 0.5 * powf(v1[cursor] - vmiu[cursor], 2) / vcov[cursor];
        cursor--;
    }
    return sqrt(sum);
}

bool HMMModelSGM::import(std::string path){
    std::ifstream ifs(path);
    
    std::string label, stringvalue;
    
    float floatvalue;
    
    int i, j;
    
    while (ifs>>label) {
        if (label.compare("[TYPE]") == 0) {
            ifs>>stringvalue;
            if (stringvalue.compare("SGM") != 0) {
                printf("%s is not a Single Gaussian Model!", path.c_str());
                return false;
            }
        }else if (label.compare("[K]") == 0){
            ifs>>K;
        }else if (label.compare("[FEATURE_LENGTH]") == 0){
            ifs>>FEATURE_LENGTH;
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
            miu = new std::vector<MFCC_Feature>();
            for (i = 0; i < K; i++) {
                MFCC_Feature * temp = new std::vector<float>();
                for (j = 0; j < FEATURE_LENGTH; j++) {
                    ifs>>floatvalue;
                    temp->push_back(floatvalue);
                }
                miu->push_back(*temp);
            }
        }else if (label.compare("[COV]") == 0){
            cov = new std::vector<MFCC_Feature>();
            for (i = 0; i < K; i++) {
                MFCC_Feature * temp = new std::vector<float>();
                for (j = 0; j < FEATURE_LENGTH; j++) {
                    ifs>>floatvalue;
                    temp->push_back(floatvalue);
                }
                cov->push_back(*temp);
            }
        }
    }
    
    ifs.close();
    
    if (cov && miu && transition_cost && entry_cost) {
        return true;
    }else{
        clear();
        printf("%s is not a complete Single Gaussian Model file!", path.c_str());
        return false;
    }
}