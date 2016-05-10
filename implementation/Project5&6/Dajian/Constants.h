//
//  Constants.h
//  speech3
//
//  Created by Dajian on 15/11/25.
//  Copyright © 2015年 Dajian. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#define MAX_IN_DTW (1000000)
#define MCOV (0.00001)
#define PI (3.14159)
#define SAMPLE_RATE   (16000)
#define FRAMES_PER_BUFFER  (320)
#define MAX_SILENCE_LENGTH   (80000)
#define FORGET_FACTOR (2)
#define ENTRY_THRESHOLD (9)
#define EXIT_THRESHOLD (6)
#define VALID_SPEECH_CHUNKS (10)
#define SILENCE_CHUNK_RETAINED (13)
#define BACKGROUND_ADJUSTMENT (0.05)
#define MFCC_WIDTH (320)
#define MFCC_STEP (160)
#define FFT_ORDER (512)
#define UNIQ_FFT_COMPONENT (257)
#define NUM_FILTER (40)
#define MFCC_FEATURE_LENGTH (39)
#define NUM_DCT_COEFFICIENT (13)
#define LOOP_BACK_COST (500)
#define CONTINUOUS_WORD_HOPPING_COST (0)


#define MFCC_Feature std::vector<float>

// raw data collected by PortAudio
struct RawRecordData{
    std::vector<short> data;
    int samplerate = 16000;
    int start = 0;
    int end = 0;
};

#endif /* Constants_h */
