//
//  SegmentalKmeansGMM.cpp
//  speech3
//
//  Created by Dajian on 15/10/9.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#include "SegmentalKmeansGMM.h"


SegmentalKmeansGMM::SegmentalKmeansGMM(){
    FEATURE_LENGTH = 39;
    MAX_ITERATIONS = 100;
    MIN_DELTA = 1e-3;
    KERNEL_NUMBER = 4;
    miu = NULL;
    cov = NULL;
    entry_cost = NULL;
    transition_cost = NULL;
    alpha = NULL;
}

SegmentalKmeansGMM::~SegmentalKmeansGMM(){
    clear();
}

void SegmentalKmeansGMM::clear(){
    if (miu) {
        delete miu;
        miu = NULL;
    }
    if (cov) {
        delete cov;
        cov = NULL;
    }
    if (transition_cost) {
        delete transition_cost;
        transition_cost = NULL;
    }
    if (entry_cost) {
        delete entry_cost;
        entry_cost = NULL;
    }
    if (alpha) {
        delete alpha;
        alpha = NULL;
    }
}

void SegmentalKmeansGMM::setMaxIteration(int max_iteration){
    MAX_ITERATIONS = max_iteration;
}

void SegmentalKmeansGMM::setMinDelta(int min_delta){
    MIN_DELTA = min_delta;
}

MFCC_Feature & SegmentalKmeansGMM::addBToA(MFCC_Feature & A, const MFCC_Feature  & B){
    long cursor = FEATURE_LENGTH - 1;
    while (cursor >= 0) {
        A[cursor] += B[cursor];
        cursor--;
    }
    return A;
}

MFCC_Feature & SegmentalKmeansGMM::divideAByB(MFCC_Feature & A, long B){
    long cursor = A.size() - 1;
    while (cursor >= 0) {
        A[cursor] = A[cursor] / B;
        cursor--;
    }
    return A;
}

void SegmentalKmeansGMM::setParameters(int K, int FEATURE_LENGTH, int KERNEL_NUMBER){
    clear();
    int i = 0, j = 0;
    float temp_float = 0;
    this->K = K;
    this->FEATURE_LENGTH = FEATURE_LENGTH;
    this->KERNEL_NUMBER = KERNEL_NUMBER;
    
    // initialize entry_cost and transition_cost
    temp_float = log(K);
    
    entry_cost = new std::vector<float>(K, temp_float);
    
    transition_cost = new std::vector<std::vector<float>>(K);
    
    for (i = 0; i < K; i++) {
        MFCC_Feature & temp = (*transition_cost)[i];
        temp.resize(K, temp_float);
    }
    
    // initialize alpha
    temp_float = 1.0 / KERNEL_NUMBER;
    alpha = new std::vector<std::vector<float>>(K);
    for (i = 0; i < K; i++) {
        std::vector<float> & ref_alpha_1 = (*alpha)[i];
        for (j = 0; j < KERNEL_NUMBER; j++) {
            ref_alpha_1.push_back(temp_float);
        }
    }
    
    // NOTICE: miu, cov are NOT initialized!
    
}

float SegmentalKmeansGMM::EuclideanDistance(const MFCC_Feature &A, const MFCC_Feature &B){
    int sum = 0;
    long cursor = FEATURE_LENGTH - 1;
    while (cursor >= 0) {
        sum += powf(A[cursor] - B[cursor], 2);
        cursor--;
    }
    return sum;
}

void SegmentalKmeansGMM::initializeGMMParameters(std::vector<std::vector<MFCC_Feature>> & container){
    
    int i = 0, j = 0, k = 0, l = 0;
    int num_iter = 0;
    long num_frames = 0, num_frames1 = 0;
    float temp_float = 0;
    int temp_int = 0;
    int min_ind = 0;
    float min_float = 0;
    
    if (miu) {
        delete miu;
        delete cov;
    }
    
    miu = new std::vector<std::vector<MFCC_Feature>>(K);
    cov = new std::vector<std::vector<std::vector<float>>>(K);
    
    for (i = 0; i < K; i++) {
        
        std::vector<MFCC_Feature> & ref_miu_1 = (*miu)[i];
        std::vector<std::vector<float>> & ref_cov_1 = (*cov)[i];
        std::vector<std::vector<float>> & ref_container_1 = container[i];
        
        num_frames = ref_container_1.size();
        
        std::vector<std::vector<MFCC_Feature>> container_kmeans(KERNEL_NUMBER);
        
        temp_int = floorf((float)num_frames / KERNEL_NUMBER);
        
        k = 0;
        
        // initialize kmean centroids
        for (j = 0; j < KERNEL_NUMBER; j++) {
            ref_miu_1.push_back(ref_container_1[k + rand() % temp_int]);
            k += temp_int;
        }
        
        // initialize cov
        ref_cov_1.resize(KERNEL_NUMBER);
        for (j = 0; j < KERNEL_NUMBER; j++) {
            ref_cov_1[j].resize(FEATURE_LENGTH, 0);
        }
        
        num_iter = 0;
        
        // kmeans
        while (num_iter < MAX_ITERATIONS) {
            
            // clear container
            for (j = 0; j < KERNEL_NUMBER; j++) {
                container_kmeans[j].clear();
            }
            
            std::vector<MFCC_Feature> new_miu(KERNEL_NUMBER);
            for (j = 0; j < KERNEL_NUMBER; j++) {
                new_miu[j].resize(FEATURE_LENGTH, 0);
            }
            
            std::vector<float> distance(KERNEL_NUMBER);
            
            for (j = 0; j < num_frames; j++) {
                // distance to centroids
                for (k = 0; k < KERNEL_NUMBER; k++) {
                    distance[k] = EuclideanDistance(ref_miu_1[k], ref_container_1[j]);
                }
                
                // find the min
                min_ind = 0;
                min_float = distance[0];
                
                for (k = 1; k < KERNEL_NUMBER; k++) {
                    temp_float = distance[k];
                    if (temp_float < min_float) {
                        min_float = temp_float;
                        min_ind = k;
                    }
                }
                
                // add to containerkmeans and new miu
                addBToA(new_miu[min_ind], ref_container_1[j]);
                container_kmeans[min_ind].push_back(ref_container_1[j]);
            }
            
            // reestimate miu
            for (j = 0; j < KERNEL_NUMBER; j++) {
                divideAByB(new_miu[j], container_kmeans[j].size());
            }
            
            temp_float = 0;
            for (j = 0; j < KERNEL_NUMBER; j++) {
                temp_float += EuclideanDistance(new_miu[j], ref_miu_1[j]);
            }
            
            for (j = 0; j < KERNEL_NUMBER; j++) {
                ref_miu_1[j] = new_miu[j];
            }
            
            if (temp_float < MIN_DELTA) {
                break;
            }
            
            num_iter++;
        }
        
        // estimate covariance
        for (j = 0; j < KERNEL_NUMBER; j++) {
            num_frames1 = container_kmeans[j].size();
            for (k = 0; k < FEATURE_LENGTH; k++) {
                if (num_frames1 == 1) {
                    ref_cov_1[j][k] = MCOV;
                }else{
                    for (l = 0; l < num_frames1; l++) {
                        ref_cov_1[j][k] += powf(container_kmeans[j][l][k] - ref_miu_1[j][k], 2);
                    }
                }
            }
            divideAByB(ref_cov_1[j], num_frames1);
        }
        
    }
    
    
}


float SegmentalKmeansGMM::distanceMFCC(const MFCC_Feature & v1, int state){
    // state range: 1-
    std::vector<MFCC_Feature> & vmiu = (*miu)[state-1];
    std::vector<std::vector<float>> & vcov = (*cov)[state-1];
    std::vector<float> & valpha = (*alpha)[state-1];
    float temp_float = 0,temp_sum = logf(valpha[0]) + LogGaussianProbability(v1, vmiu[0], vcov[0]);
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

// P(xi | thetaj)
double SegmentalKmeansGMM::LogGaussianProbability(const MFCC_Feature & xi, MFCC_Feature & miul, std::vector<float> & covl){
    double logp = 0;
    for (int i = 0; i < FEATURE_LENGTH; i++) {
        logp = logp - 0.5 * log(2 * PI * covl[i]) - 0.5 * pow(xi[i]-miul[i],2) / covl[i];
    }
    return logp;
}

void SegmentalKmeansGMM::estimateGMM(std::vector<std::vector<MFCC_Feature>> &container){
    // reference: http://blog.csdn.net/abcjennifer/article/details/8198352
    
    int i = 0, j = 0, k = 0,l = 0;
    int num_iter = 0;
    long num_frames = 0;
    double L = 0, lastL = 0;
    double sum_double = 0, temp_double = 0;
    
    //temp_double = exp(740);
    
    if (!miu) {
        initializeGMMParameters(container);
    }
    
    // EM estimate for each state
    for (i = 0; i < K; i++) {
        std::vector<MFCC_Feature> & ref_miu_1 = (*miu)[i];
        std::vector<std::vector<float>> & ref_cov_1 = (*cov)[i];
        std::vector<float> & ref_alpha_1 = (*alpha)[i];
        std::vector<std::vector<float>> & ref_container_1 = container[i];
        
        num_frames = ref_container_1.size();
        L = 0;
        num_iter = 0;
        
        while (num_iter < MAX_ITERATIONS) {
            
            num_iter++;
        
            std::vector<std::vector<double>> px(num_frames);
            
            lastL = L;
            
            // estimate step
            
            for (j = 0; j < num_frames; j++) {
                px[j].resize(KERNEL_NUMBER, 0);
            }
            
            // probability that one point belongs to one kernel P(l | xi, theta)
            for (j = 0; j < num_frames; j++) {
                temp_double = LogGaussianProbability(ref_container_1[j], ref_miu_1[0], ref_cov_1[0]) + logf(ref_alpha_1[0]);
                px[j][0] = temp_double; // log( P( xi | theta_0) * alpha_0 )
                sum_double = temp_double;
                for (k = 1; k < KERNEL_NUMBER; k++) {
                    temp_double = LogGaussianProbability(ref_container_1[j], ref_miu_1[k], ref_cov_1[k]) + log(ref_alpha_1[k]);
                    px[j][k] = temp_double; // log( P( xi | theta_l) * alpha_l )
                    if (temp_double - sum_double > 10) {
                        sum_double = temp_double;
                    }else{
                        sum_double += log(1 + exp(temp_double - sum_double));
                    }
                }
                for (k = 0; k < KERNEL_NUMBER; k++) {
                    px[j][k]= px[j][k] - sum_double; // log( P ( l | xi, theta ) )
                }
            }
            
            // re-estimate alpha and miu
            for (j = 0; j < KERNEL_NUMBER; j++) {
                sum_double = px[0][j]; // alpha_l = log( sum( P(l | xi, theta) ) )
                for (k = 1; k < num_frames; k++) {
                    if (px[k][j] - sum_double > 10) {
                        sum_double = px[k][j];
                    }else{
                        sum_double += log(1 + exp(px[k][j] - sum_double));
                    }
                }
                ref_alpha_1[j] = exp(sum_double - log(num_frames)); // alpha
                
                
                for (k = 0; k < FEATURE_LENGTH; k++) {
                    ref_miu_1[j][k] = 0;
                    for (l = 0; l < num_frames; l++) {
                        if (ref_container_1[l][k] < 0) {
                            ref_miu_1[j][k] -= exp(log(-ref_container_1[l][k])+px[l][j]);
                        }else if(ref_container_1[l][k] > 0){
                            ref_miu_1[j][k] += exp(log(ref_container_1[l][k])+px[l][j]);
                        }
                    }
                    ref_miu_1[j][k] = ref_miu_1[j][k] / exp(sum_double); // miu
                }
                
                for (k = 0; k < FEATURE_LENGTH; k++) {
                    if (ref_container_1[0][k] - ref_miu_1[j][k] == 0) {
                        ref_cov_1[j][k] = -2000 + px[0][j];
                    }else{
                        ref_cov_1[j][k] = log(pow(ref_container_1[0][k] - ref_miu_1[j][k] , 2)) + px[0][j];
                    }
                    for (l = 1; l < num_frames; l++) {
                        if (ref_container_1[l][k] - ref_miu_1[j][k] == 0) {
                            temp_double = -2000 + px[0][j];
                        }else{
                            temp_double = log(pow(ref_container_1[l][k] - ref_miu_1[j][k], 2)) + px[l][j];
                        }
                        if (temp_double - ref_cov_1[j][k] > 10) {
                            ref_cov_1[j][k] = temp_double;
                        }else{
                            ref_cov_1[j][k] += log(1 + exp(temp_double - ref_cov_1[j][k]));
                        }
                    }
                    ref_cov_1[j][k] = exp(ref_cov_1[j][k] - sum_double); // cov
                    if (ref_cov_1[j][k] == 0) {
                        ref_cov_1[j][k] = 0.00001;
                    }
                }
            }
            
            // convergence test with miu
            L = 0;
            for (j = 0; j < KERNEL_NUMBER; j++) {
                for (k = 0; k < FEATURE_LENGTH; k++) {
                    L += fabs(ref_miu_1[j][k]);
                }
            }
            
            if (fabs(L-lastL) < MIN_DELTA) {
                printf("EM: State %d Converge in %d iterations.\n", i, num_iter);
                break;
            }
            
        }
        
    }
    
}

float SegmentalKmeansGMM::getCost(int s1, int s2){
    if (s1==0) {
        return (*entry_cost)[s2-1];
    }else{
        return (*transition_cost)[s1-1][s2-1];
    }
}

// get # of states
int SegmentalKmeansGMM::getNumberOfStates(){
    return K;
}

void SegmentalKmeansGMM::commit(const std::vector<std::vector<MFCC_Feature>> samples){
    if (!alpha) {
        printf("Please set parameters first.\n");
        return;
    }
    
    // useful variables
    int i,j,k,min_ind;
    long num_files = samples.size();
    long num_frames;
    int num_iter = 0;
    float temp_float;
    float min;
    int total_cost = 0, last_cost; // convergence test
    int temp_int, temp_int2,cursor;
    std::vector<MFCC_Feature>::const_iterator iter_MFCC;
    MFCC_Feature sum(FEATURE_LENGTH, 0);
    
    // container for each cluster
    std::vector<std::vector<MFCC_Feature>> containers(K); // K | scalable | FEATURE_LENGTH
    
    
    // initialize parameters
    
    // first loop over all frames, allocate frames for initialization
    for (i = 0; i < num_files; i++) {
        const std::vector<MFCC_Feature> & temp_ref_vector = samples[i];
        temp_int = ceil(double(samples[i].size()) / K); // # of frames allocated to a state initially
        cursor = 0;
        temp_int2 = 0;
        for (iter_MFCC = temp_ref_vector.cbegin(); iter_MFCC != temp_ref_vector.cend(); iter_MFCC++) {
            containers[cursor].push_back(*iter_MFCC); // put into containers
            temp_int2++;
            if (temp_int2==temp_int) {
                cursor++;
                temp_int2 = 0;
            }
        }
    }
    
    printf("Initializing GMM...\n");
    
    // estimate states distribution
    estimateGMM(containers);
    
    // start iteration
    while (num_iter < MAX_ITERATIONS) {
        
        num_iter++;
        
        last_cost = total_cost;
        total_cost = 0;
        
        printf("The %d iteration.\n", num_iter);
        
        // clear containers
        for (i = 0; i < K; i++) {
            containers[i].clear();
        }
        
        // set up state transition matrix
        std::vector<std::vector<int>> transition_matrix(K);
        for (i = 0; i < K; i++) {
            transition_matrix[i].resize(K, 0);
        }
        
        // set up entry vector
        std::vector<int> entry_matrix(K, 0);
        
        // DTW to find the route, put frames into containers, add up containers to miu
        for (i = 0; i < num_files; i++) {
            
            // DTW
            
            const std::vector<MFCC_Feature> & temp_ref_vector = samples[i];
            
            num_frames = temp_ref_vector.size();
            std::vector<std::vector<float>> trellis(K + 1); // DTW trellis
            std::vector<std::vector<int>> next(K + 1); // mark the next state to found
            
            for (j = 0; j <= K; j++) {
                next[j].resize(num_frames + 1, 0);
            }
            
            // first column
            trellis[0].push_back(0);
            next[0][0] = -1;
            for (k = 1; k <= K; k++) {
                trellis[k].push_back(INFINITY);
            }
            
            // other columns
            for (j = 0; j < num_frames; j++) {
                // first rows
                trellis[0].push_back(INFINITY);
                // other rows
                for (k = 1; k <= K; k++) {
                    if (trellis[k][j] == INFINITY) {
                        min = INFINITY;
                    }else{
                        min = trellis[k][j] + (*transition_cost)[k-1][k-1];
                    }
                    min_ind = k;
                    
                    if (trellis[k-1][j] == INFINITY) {
                        temp_float = INFINITY;
                    }else if (k >= 2) {
                        temp_float = trellis[k-1][j] + (*transition_cost)[k-2][k-1];
                    }else{
                        temp_float = trellis[k-1][j] + (*entry_cost)[k-1];
                    }
                    if (temp_float < min) {
                        min = temp_float;
                        min_ind = k - 1;
                    }
                    
                    if (k >= 2) {
                        if (trellis[k-2][j] == INFINITY) {
                            temp_float = INFINITY;
                        }else if (k >= 3) {
                            temp_float = trellis[k-2][j] + (*transition_cost)[k-3][k-1];
                        }else{
                            temp_float = trellis[k-2][j] + (*entry_cost)[k-1];
                        }
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = k - 2;
                        }

                    }
                    
                    trellis[k].push_back(min + distanceMFCC(temp_ref_vector[j], k));
                    next[k][j + 1] = min_ind;
                }
            }
            
            // find the route stored in 'next'
            long cursor_frame = num_frames;
            int cursor_state = K;
            while (next[cursor_state][cursor_frame] != -1) {
                containers[cursor_state - 1].push_back(temp_ref_vector[cursor_frame-1]); // put into containers
                int next_cursor_state = next[cursor_state][cursor_frame];
                if (next_cursor_state == 0) {
                    entry_matrix[cursor_state-1]++; // add to entry matrix
                }else{
                    transition_matrix[next_cursor_state-1][cursor_state-1]++; // add to transition matrix
                }
                cursor_state = next_cursor_state;
                total_cost += cursor_state;
                cursor_frame--;
            }
            
        }
        
        // re-estimate transition cost
        for (i = 0; i < K; i++) {
            for (j = 0; j < K; j++) {
                if (transition_matrix[i][j] == 0) {
                    (*transition_cost)[i][j] = MAX_IN_DTW;
                }else{
                    (*transition_cost)[i][j] = -log((float)transition_matrix[i][j]  / containers[i].size());
                }
                
            }
        }
        
        // re-estimate entry cost
        for (i = 0; i < K; i++) {
            if (entry_matrix[i] == 0) {
                (*entry_cost)[i] = MAX_IN_DTW;
            }else{
                (*entry_cost)[i] = -log(float(entry_matrix[i]) / num_files);
            }
        }
        
        // re-estimate state distribution
        estimateGMM(containers);
        
        if (total_cost == last_cost) {
            break;
        }
    }
    
    if (num_iter == MAX_ITERATIONS) {
        printf("Fail to converge in %d iterations.\n", num_iter);
    }else{
        printf("Converge in %d iterations.\n", num_iter);
    }
    
}

void SegmentalKmeansGMM::output(std::string path){
    if (!miu) {
        return;
    }
    
    printf("Output model!\n");
    
    int i,j,k;
    
    std::ofstream ofs;
    
    ofs.open(path);
    
    ofs<<"[TYPE]\nGMM\n";
    
    ofs<<"[K]\n"<<K<<"\n";
    
    ofs<<"[FEATURE_LENGTH]\n"<<FEATURE_LENGTH<<"\n";
    
    ofs<<"[KERNEL_NUMBER]\n"<<KERNEL_NUMBER<<"\n";
    
    ofs<<"[ENTRY_COST]\n";
    
    for (i = 0; i < K; i++) {
        ofs<<(*entry_cost)[i]<<" ";
    }
    
    ofs<<"\n";
    
    ofs<<"[TRANSITION_COST]\n";
    
    for (i = 0; i < K; i++) {
        for (j = 0; j < K; j++) {
            ofs<<(*transition_cost)[i][j]<<" ";
        }
        ofs<<"\n";
    }
    
    ofs<<"[MIU]\n";
    
    for (i = 0; i < K; i++) {
        for (j = 0; j < KERNEL_NUMBER; j++) {
            for (k = 0; k < FEATURE_LENGTH; k++) {
                ofs<<(*miu)[i][j][k]<<" ";
            }
            ofs<<"\n";
        }
        ofs<<"\n";
    }
    
    ofs<<"[COV]\n";
    
    for (i = 0; i < K; i++) {
        for (j = 0; j < KERNEL_NUMBER; j++) {
            for (k = 0; k < FEATURE_LENGTH; k++) {
                ofs<<(*cov)[i][j][k]<<" ";
            }
            ofs<<"\n";
        }
        ofs<<"\n";
    }
    
    ofs<<"[ALPHA]\n";
    
    for (i = 0; i < K; i++) {
        for (j = 0; j < KERNEL_NUMBER; j++) {
            ofs<<(*alpha)[i][j]<<" ";
        }
        ofs<<"\n";
    }
    
    ofs.flush();
    
    ofs.close();
    
}



