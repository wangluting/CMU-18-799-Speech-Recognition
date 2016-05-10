#define NUM_FILTER 40 

#ifndef MFCC_H
#define MFCC_H

void preEmphasize(float* preSample, short *sample, int numSamples, int factor);

void FFT(float* in, float*energySpectrum);

void computeMel(float* mel, int sampleRate, float* energySpectrum);

void DCT(const float *mel, float *cepstrum);

void IDCT(double *in, double *out);


#endif