#include <math.h>
#include "mfcc.h"
#include "fftw3.h"


void preEmphasize(float* preSample, short *sample, int numSamples, int factor) //Ô¤¼ÓÖØ
{
	preSample[0] = sample[0];
	for (int i = 1; i < numSamples; i++)
	{
		preSample[i] = sample[i] - factor*sample[i - 1];
	}
}

void FFT(float* in, float* energySpectrum)
{
	fftwf_complex *out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * 512); 
	fftwf_plan p = fftwf_plan_dft_r2c_1d(512, in, out, FFTW_ESTIMATE);
	fftwf_execute(p);
	for (int i = 0; i < 257; i++)
	{
		energySpectrum[i] = out[i][0] * out[i][0] + out[i][1] * out[i][1];
	}
	fftwf_destroy_plan(p);
	fftwf_free(out);
}

void computeMel(float* mel, int sampleRate, float* energySpectrum)
{
	int fmax = sampleRate / 2;
	float maxMelFreq = 1127 * log(1 + fmax / 700);
	float delta = maxMelFreq / (NUM_FILTER + 1);
	float melFilter[NUM_FILTER][3];
	float *m = new float[NUM_FILTER + 2];
	float *n = new float[NUM_FILTER + 2];
	for (int i = 0; i < NUM_FILTER + 2; i++)
	{
		m[i] = 700 * (exp(i*delta / 1127) - 1);
		n[i] = (m[i] / fmax) * (256 + 1);
	}
	for (int i = 0; i < NUM_FILTER; i++)
	{
		melFilter[i][0] = n[i];
		melFilter[i][1] = n[i + 1];
		melFilter[i][2] = n[i + 2];
	}
	delete[] m;
	delete[] n;

	for (int i = 0; i < NUM_FILTER; i++)
	{
		for (int j = 0; j < (256+1); j++)
		{
			if (j >= melFilter[i][0] && j < melFilter[i][1])
			{
				mel[i] += ((j - melFilter[i][0]) / (melFilter[i][1] - melFilter[i][0])) * energySpectrum[j];
			}
			if (j >= melFilter[i][1] && j < melFilter[i][2])
			{
				mel[i] += ((j - melFilter[i][2]) / (melFilter[i][1] - melFilter[i][2])) * energySpectrum[j];
			}
		}
	}
}

void DCT(const float *mel, float *cepstrum)
{
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < NUM_FILTER; j++)
		{
			cepstrum[i] += mel[j]*cos(3.14*i / (2 * NUM_FILTER)*(2 * j + 1));
		}
	}
}

void IDCT(double *in, double *out) 
{
	fftw_plan p = fftw_plan_r2r_1d(40, in, out, FFTW_REDFT01, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
}
