//
//  DTWGallery.cpp
//  speech3
//
//  Created by Dajian on 15/11/25.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#include "DTWGallery.hpp"


//
int DTWGallery::DTW_single_digit(std::vector<HMMModel *> dictionary, std::vector<MFCC_Feature> & sample){
    
    int i,j,tempint1,tempint2,min_ind;
    float min,temp_float;
    
    
    // set up map from trellis to dictionary
    
    std::vector<From_Model_State> trellis_to_dictionary;
    
    tempint1 = (int)dictionary.size();
    
    for (i = 0; i < tempint1; i++) {
        tempint2 = dictionary[i]->getNumberOfStates();
        From_Model_State temp1(NULL, 0, 0);
        trellis_to_dictionary.push_back(temp1); // dummy state
        for (j = 1; j <= tempint2; j++) {
            From_Model_State temp2(dictionary[i], i, j); // non-dummy state
            trellis_to_dictionary.push_back(temp2);
        }
    }
    
    int num_states = (int)trellis_to_dictionary.size();
    int num_frames = (int)sample.size();
    
    std::vector<std::vector<float>> trellis(num_states);
    
    // first column
    for (i = 0; i < num_states; i++) {
        if (trellis_to_dictionary[i].model == NULL) {
            trellis[i].push_back(0);
        }else{
            trellis[i].push_back(MAX_IN_DTW);
        }
        
    }
    
    for (i = 0; i < num_frames; i++) {
        for (j = 0; j < num_states; j++) {
            if (trellis_to_dictionary[j].model == NULL) {
                trellis[j].push_back(MAX_IN_DTW);
            }else{
                
                int this_state = trellis_to_dictionary[j].index_State;

                if (trellis[j][i] >= MAX_IN_DTW) {
                    min = MAX_IN_DTW;
                }else{
                    min = trellis[j][i] + trellis_to_dictionary[j].model->getCost(this_state, this_state);
                }
                
                if (trellis[j-1][i] >= MAX_IN_DTW) {
                    temp_float = MAX_IN_DTW;
                }else if (this_state >= 2) {
                    temp_float = trellis[j-1][i] + trellis_to_dictionary[j].model->getCost(this_state - 1, this_state);
                }else{
                    temp_float = trellis[j-1][i] + trellis_to_dictionary[j].model->getCost(0, this_state);
                }
                
                if (temp_float < min) {
                    min = temp_float;
                }
                
                if (this_state >= 2) {
                    if (trellis[j-2][i] >= MAX_IN_DTW) {
                        temp_float = MAX_IN_DTW;
                    }else if (this_state >= 3) {
                        temp_float = trellis[j-2][i] + trellis_to_dictionary[j].model->getCost(this_state - 2, this_state);;
                    }else{
                        temp_float = trellis[j-2][i] + trellis_to_dictionary[j].model->getCost(0, this_state);
                    }
                    
                    if (temp_float < min) {
                        min = temp_float;
                    }
                }
                
                
                if (min >= MAX_IN_DTW) {
                    trellis[j].push_back(MAX_IN_DTW);
                }else{
                    trellis[j].push_back(min + trellis_to_dictionary[j].model->distanceMFCC(sample[i], this_state));
                }
                
            }
        }
    }
    
    tempint1 = dictionary[0]->getNumberOfStates();
    min = trellis[tempint1][num_frames];
    min_ind = 0;
    
    tempint2 = (int)dictionary.size();
    
    for (i = 1; i < tempint2; i++) {
        tempint1 += 1 + dictionary[i]->getNumberOfStates();
        temp_float = trellis[tempint1][num_frames];
        if (min > temp_float) {
            min = temp_float;
            min_ind = i;
        }
    }
    
    return min_ind;
}


void DTWGallery::DTW_sequence_structure1(std::vector<HMMModel *> dictionary, std::vector<MFCC_Feature> & sample){
    
    int i,j,tempint1 = 0,tempint2 = 0,min_ind = 0;
    float min,temp_float = 0;
    
    // set up map from trellis to dictionary
    
    std::vector<From_Model_State> trellis_to_dictionary;
    
    From_Model_State start(NULL, -1, -1); // non-emitting state: start
    start.from = new std::vector<int>();
    
    trellis_to_dictionary.push_back(start); // suppose shallow copy
    
    int dictionary_size = (int)dictionary.size();
    
    for (i = 0; i < dictionary_size; i++) {
        tempint2 = dictionary[i]->getNumberOfStates();
        
        // the first emitting state of the model, from start
        From_Model_State temp1(dictionary[i], i, 1);
//        temp1.from = new std::vector<int>();
//        temp1.from->push_back(0);
        trellis_to_dictionary.push_back(temp1);
        
        for (j = 2; j <= tempint2; j++) {
            From_Model_State temp2(dictionary[i], i, j); // emitting state
            trellis_to_dictionary.push_back(temp2);
        }
        
        start.from->push_back((int)trellis_to_dictionary.size() - 1);
    }
    
    int num_states = (int)trellis_to_dictionary.size();
    
    int num_frames = (int)sample.size();
    
    // now start calculation
    std::vector<BackPointerTableElement> backpointertable;
    
    BackPointerTableElement tableelement_start;
    tableelement_start.index_Model = -1;
    tableelement_start.from_index_in_table = -1;
    backpointertable.push_back(tableelement_start);
    
    float * trellis_column = new float[num_states]; // trellis at current frame
    
    int * parents = new int[num_states]; // parents (index of the parent in back pointer table) at current frame
    
    // initialize trellis_column with the first frame
    for (i = 1; i < num_states; i++) {
        switch (trellis_to_dictionary[i].index_State) {
            case 1:
                trellis_column[i] = trellis_to_dictionary[i].model->distanceMFCC(sample[0], 1) + trellis_to_dictionary[i].model->getCost(0, 1);
                parents[i] = 0;
                break;
            case 2:
                trellis_column[i] = trellis_to_dictionary[i].model->distanceMFCC(sample[0], 2) + trellis_to_dictionary[i].model->getCost(0, 2);
                parents[i] = 0;
                break;
            default:
                trellis_column[i] = MAX_IN_DTW;
                parents[i] = -1;
                break;
        }
    }
    
    // initialize the start
    trellis_column[0] = MAX_IN_DTW;
    parents[0] = -1;
    for (i = 0; i < dictionary_size; i++) {
        int this_ind = (*start.from)[i];
        if (trellis_column[this_ind] < trellis_column[0]) {
            trellis_column[0] = trellis_column[this_ind] + LOOP_BACK_COST; // add loop back cost
            parents[0] = parents[this_ind];
            tempint1 = this_ind; // temperarily store the index
        }
    }
    
    // add to back pointer table if one model reaches its leaf
    if (trellis_column[0] < MAX_IN_DTW) {
        BackPointerTableElement tempelement;
        tempelement.index_Model = trellis_to_dictionary[tempint1].index_Model;
        tempelement.from_index_in_table = parents[0];
        parents[0] = (int)backpointertable.size(); // set to index of this element
        backpointertable.push_back(tempelement);
    }
    
//    for (i = 0; i < num_states; i++) {
//        std::cout<<trellis_column[i]<<"\t"<<parents[i]<<"\n";
//    }
    
    
    for (i = 1; i < num_frames; i++) {
        
        float * temp_trellis_column = new float[num_states];
        int * temp_parents = new int[num_states];
        
        for (j = 1; j < num_states; j++) {
            
            int this_state = trellis_to_dictionary[j].index_State;
            
            min = trellis_column[j] + trellis_to_dictionary[j].model->getCost(this_state, this_state);
            min_ind = j;
            
            if (this_state >= 2) { // others, always from the former state
                if (trellis_column[j-1] >= MAX_IN_DTW) {
                    temp_float = MAX_IN_DTW;
                } else{
                    temp_float = trellis_column[j-1] + trellis_to_dictionary[j].model->getCost(this_state-1, this_state);
                    if (temp_float < min) {
                        min = temp_float;
                        min_ind = j-1;
                    }
                }
                
                if (this_state == 2) { // the 2nd state, can from 'start'
                    if (trellis_column[0] >= MAX_IN_DTW) {
                        temp_float = MAX_IN_DTW;
                    } else{
                        temp_float = trellis_column[0] + trellis_to_dictionary[j].model->getCost(0, 2);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = 0;
                        }
                    }
                }else{ // others, always from the 2nd former state
                    if (trellis_column[j-2] >= MAX_IN_DTW) {
                        temp_float = MAX_IN_DTW;
                    } else{
                        temp_float = trellis_column[j-2] + trellis_to_dictionary[j].model->getCost(this_state-2, this_state);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j-2;
                        }
                    }
                }
                
            } else{ // the 1st state, can from 'state'
                if (trellis_column[0] >= MAX_IN_DTW) {
                    temp_float = MAX_IN_DTW;
                } else{
                    temp_float = trellis_column[0] + trellis_to_dictionary[j].model->getCost(0, 1);
                    if (temp_float < min) {
                        min = temp_float;
                        min_ind = 0;
                    }
                }
            }
            
            if (min >= MAX_IN_DTW) {
                temp_trellis_column[j] = min;
            }else{
                temp_trellis_column[j] = min + trellis_to_dictionary[j].model->distanceMFCC(sample[i], this_state);
            }
            temp_parents[j] = parents[min_ind];
            
        }
        
        // copy result to trellis_column and parents
        for (j = 1; j < num_states; j++) {
            trellis_column[j] = temp_trellis_column[j];
            parents[j] = temp_parents[j];
        }
        
        // the 'start'
        trellis_column[0] = MAX_IN_DTW;
        parents[0] = -1;
        for (j = 0; j < dictionary_size; j++) {
            int this_ind = (*start.from)[j];
            if (trellis_column[this_ind] < trellis_column[0]) {
                trellis_column[0] = trellis_column[this_ind] + LOOP_BACK_COST; // add loop back cost
                parents[0] = parents[this_ind];
                tempint1 = this_ind; // temperarily store the index
            }
        }
        
        // add to back pointer table if one model reaches its leaf
        if (trellis_column[0] < MAX_IN_DTW) {
            BackPointerTableElement tempelement;
            tempelement.index_Model = trellis_to_dictionary[tempint1].index_Model;
            tempelement.from_index_in_table = parents[0];
            parents[0] = (int)backpointertable.size(); // set to index of this element
            backpointertable.push_back(tempelement);
        }
        
//        std::cout<<"--------------------"<<i<<"-----------------\n";
//        
//        for (j = 0; j < num_states; j++) {
//            std::cout<<trellis_column[j]<<"\t"<<parents[j]<<"\n";
//        }
        
        delete [] temp_trellis_column;
        delete [] temp_parents;
        
    }
    
    // trace back in back pointer table
    std::vector<int> result;
    tempint1 = (int)backpointertable.size() - 1;
    while (backpointertable[tempint1].from_index_in_table != -1) {
        result.push_back(backpointertable[tempint1].index_Model);
        tempint1 = backpointertable[tempint1].from_index_in_table;
    }
    
    while (!result.empty()) {
        std::cout<< result.back() << std::endl;
        result.pop_back();
    }
    
    // clear heap
//    for (i = 0; i < num_states; i++) {
//        if (trellis_to_dictionary[i].from) {
//            delete trellis_to_dictionary[i].from;
//        }
//    }
    delete start.from;
    
    delete [] parents;
    delete [] trellis_column;
    
}


void DTWGallery::DTW_sequence_structure_configured(std::vector<SegmentalKmeansGMM *> dictionary, std::vector<int> model_concat_order, std::vector<MFCC_Feature> & sample, std::vector<std::vector<std::vector<MFCC_Feature>>> & containers){
    
    int i,j,tempint1 = 0,tempint2 = 0, tempint3,min_ind = 0;
    float min = MAX_IN_DTW,temp_float = 0;
    
    // set up map from trellis to dictionary
    
    std::vector<From_TrainingGMMModel_State> trellis_to_dictionary;
    
    tempint1 = (int)model_concat_order.size();
    
    for (i = 0; i < tempint1; i++) {
        tempint2 = model_concat_order[i];
        tempint3 = dictionary[tempint2]->getNumberOfStates();
        for (j = 0; j <= tempint3; j++) {
            From_TrainingGMMModel_State temp2(dictionary[tempint2],i,j); // including non-emitting state
            trellis_to_dictionary.push_back(temp2);
        }
    }
    
    int num_states = (int)trellis_to_dictionary.size();
    int num_frames = (int)sample.size();
    
    std::vector<std::vector<float>> trellis(num_states);
    
    std::vector<std::vector<int>> parents(num_states);
    
    for (i = 0; i < num_states; i++) {
        trellis[i].resize(num_frames, MAX_IN_DTW);
        parents[i].resize(num_frames, -1);
    }
    
    // initialize the first column
    trellis[0][0] = 0;
    //parents[0][0] = -1;
    
    trellis[1][0] = trellis_to_dictionary[1].model->getCost(0, 1) + trellis_to_dictionary[1].model->distanceMFCC(sample[0], 1);
    parents[1][0] = 0;
    if (trellis_to_dictionary[2].index_State == 2) {
        trellis[2][0] = trellis_to_dictionary[2].model->getCost(0, 2) + trellis_to_dictionary[1].model->distanceMFCC(sample[0], 2);
        parents[2][0] = 0;
    }
    
    // calculate trellis
    for (i = 1; i < num_frames; i++) {
        for (j = 1; j < num_states; j++) {
            int this_state = trellis_to_dictionary[j].index_State;
            switch (this_state) {
                case 0:
                    
                    if (trellis[j-1][i-1] >= MAX_IN_DTW) {
                        trellis[j][i] = MAX_IN_DTW;
                    }else{
                        trellis[j][i] = trellis[j-1][i-1] + CONTINUOUS_WORD_HOPPING_COST; // end of word cost
                    }

                    parents[j][i] = j - 1;
                    break;
                    
                case 1:
                    // from state 1
                    if (trellis[j][i-1] >= MAX_IN_DTW) {
                        min = MAX_IN_DTW;
                    }else{
                        min = trellis[j][i-1] + trellis_to_dictionary[j].model->getCost(1, 1);
                    }
                    min_ind = j;
                    
                    // from non-emitting state
                    if (trellis[j-1][i] < MAX_IN_DTW) {
                        temp_float = trellis[j-1][i] + trellis_to_dictionary[j].model->getCost(0, 1);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j - 1;
                        }
                    }
                    
                    break;
                case 2:
                    // from state 2
                    if (trellis[j][i-1] >= MAX_IN_DTW) {
                        min = MAX_IN_DTW;
                    }else{
                        min = trellis[j][i-1] + trellis_to_dictionary[j].model->getCost(2, 2);
                    }
                    min_ind = j;
                    
                    // from state 1
                    if (trellis[j-1][i-1] < MAX_IN_DTW) {
                        temp_float = trellis[j-1][i-1] + trellis_to_dictionary[j].model->getCost(1, 2);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j - 1;
                        }
                    }
                    
                    // from non-emitting state
                    if (trellis[j-2][i] < MAX_IN_DTW) {
                        temp_float = trellis[j-2][i] + trellis_to_dictionary[j].model->getCost(0, 2);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j - 2;
                        }
                    }
                    
                    break;
                default:
                    
                    // from this_state
                    if (trellis[j][i-1] >= MAX_IN_DTW) {
                        min = MAX_IN_DTW;
                    }else{
                        min = trellis[j][i-1] + trellis_to_dictionary[j].model->getCost(this_state, this_state);
                    }
                    min_ind = j;
                    
                    // from this_state-1
                    if (trellis[j-1][i-1] < MAX_IN_DTW) {
                        temp_float = trellis[j-1][i-1] + trellis_to_dictionary[j].model->getCost(this_state-1, this_state);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j - 1;
                        }
                    }
                    
                    // from this_state-2
                    if (trellis[j-2][i-1] < MAX_IN_DTW) {
                        temp_float = trellis[j-2][i-1] + trellis_to_dictionary[j].model->getCost(this_state-2, this_state);
                        if (temp_float < min) {
                            min = temp_float;
                            min_ind = j - 2;
                        }
                    }
                    
                    
                    break;
            }
            
            if (this_state != 0) {
                if (min >= MAX_IN_DTW) {
                    break;
                }else{
                    trellis[j][i] = min + trellis_to_dictionary[j].model->distanceMFCC(sample[i], this_state);
                    parents[j][i] = min_ind;
                }
            }
            
        }
    }
    
    // trace back to find the path and put into container
    
    i = num_frames - 1;
    j = num_states - 1;
    
    while (i >=0 && parents[j][i] != -1) {
        containers[trellis_to_dictionary[j].index_Model][trellis_to_dictionary[j].index_State - 1].push_back(sample[i]);
        j = parents[j][i];
        // if emitting state, move one step more
        if (j > 0 && trellis_to_dictionary[j].index_State == 0) {
            j = parents[j][i];
        }
        i--;
    }
    
}