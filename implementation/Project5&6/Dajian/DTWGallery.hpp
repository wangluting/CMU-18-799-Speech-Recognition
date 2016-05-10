//
//  DTWGallery.hpp
//  speech3
//
//  Created by Dajian on 15/11/25.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef DTWGallery_hpp
#define DTWGallery_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "HMMModel.hpp"
#include "SegmentalKmeansGMM.hpp"

struct From_Model_State{
    HMMModel * model;
    int index_Model;
    int index_State;
    std::vector<int> * from;
    
    From_Model_State(HMMModel * model, int index_Model, int index_State){
        this->model = model;
        this->index_Model = index_Model;
        this->index_State = index_State;
        from = NULL;
    }
};

struct From_TrainingGMMModel_State{
    SegmentalKmeansGMM * model;
    int index_Model;
    int index_State;
    
    From_TrainingGMMModel_State(SegmentalKmeansGMM * model, int index_Model, int index_State){
        this->model = model;
        this->index_Model = index_Model;
        this->index_State = index_State;
    }
};

struct BackPointerTableElement{
    int index_Model;
    int from_index_in_table;
};

class DTWGallery{
    
public:
    
    // single digit without pruning
    int DTW_single_digit(std::vector<HMMModel *> dictionary, std::vector<MFCC_Feature> & sample);
    
    // digit sequence without pruning, with back pointer table, maintain 2 column: structure = unrestricted number of digits
    void DTW_sequence_structure1(std::vector<HMMModel *> dictionary, std::vector<MFCC_Feature> & sample);
    
    // digit sequence without pruning, with back pointer table, maintain 2 column: structure = unrestricted number of digits
    void DTW_sequence_structure_configured(std::vector<SegmentalKmeansGMM *> dictionary, std::vector<int> model_concat_order, std::vector<MFCC_Feature> & sample, std::vector<std::vector<std::vector<MFCC_Feature>>> & containers);
    
};

#endif /* DTWGallery_hpp */
