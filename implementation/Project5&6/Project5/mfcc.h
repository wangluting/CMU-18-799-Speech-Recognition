#include <vector>
#define NUM_FILTER 40 
#define SAMPLE_RATE 16000
using namespace std;

#ifndef MFCC_H
#define MFCC_H


void preEmphasize(vector<float> &preSample, short *sample, int numSamples, int factor);

void FFT(float* in, float*energySpectrum);

void computeMel(float* mel, int sampleRate, float* energySpectrum);

void DCT(const float *mel, vector<float> &cepstrum);

void IDCT(double *in, double *out);

void getMfccFeature(short* sample, vector<vector<float> > &mfcc39, int numSamples);


#endif