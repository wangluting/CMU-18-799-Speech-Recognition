#include <math.h>
#include "mfcc.h"
#include "fftw3.h"
#include <iostream>
#include <fstream>

using namespace std;

void preEmphasize(vector<float> &preSample, short *sample, int numSamples, int factor) //预加重
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

void DCT(const float *mel, vector<float> &cepstrum)
{
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < NUM_FILTER; j++)
		{
			cepstrum[i] += mel[j]*cos(3.14*i / (2 * NUM_FILTER)*(2 * j + 1));
			//cout << i << " " << cepstrum[i] << endl;
		}
	}
}

void IDCT(double *in, double *out) 
{
	fftw_plan p = fftw_plan_r2r_1d(40, in, out, FFTW_REDFT01, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
}

void getMfccFeature(short* sample, vector<vector<float> > &mfcc39, int numSamples)
{
	//float* preSample = new float[numSamples];
	
	vector<float> preSample(numSamples);
	preEmphasize(preSample, sample, numSamples, 0.95);

	const int numFrame = 400; //分帧
	const int notOverLap = 200;
	float hamingwindow[numFrame]; //构造汉明窗
	for (int i = 0; i < numFrame; i++)
	{
		hamingwindow[i] = 0.54 - 0.46*cos(2 * 3.14*i / numFrame);
	}
	int num = (numSamples - numFrame) / notOverLap + 1;
	vector<vector<float> >  cepstrum(num);
	for (int i = 0; i < num; i++)
	{
		cepstrum[i].resize(39);
	}
	ofstream outfile1;
	outfile1.open("mel cepstrum.txt");
	for (int n = 0; n < num; n++)
	{
		float windowSample[512];    //加窗后的采样点
		float energySpectrum[257];  //DFT后的频谱
		int i;
		for (i = 0; i < numFrame && (i + n*notOverLap)<numSamples; i++)
		{
			windowSample[i] = preSample[i + n*notOverLap] * (hamingwindow[i]);
		}
		for (int k = i; k < 512; k++)             //补零
		{
			windowSample[k] = 0;
		}
		FFT(windowSample, energySpectrum);  //离散傅里叶变换
		float mel[NUM_FILTER];               //计算mel spectrum
		memset(mel, 0, sizeof(float)*NUM_FILTER);
		int sampleRate = SAMPLE_RATE;
		computeMel(mel, sampleRate, energySpectrum);
		for (int j = 0; j < NUM_FILTER; j++)                   //转换成 log mel spectrum
		{
			//if (mel[j] <= -0.0001 || mel[j] >= 0.0001)
				mel[j] = log(mel[j]);
		}

		DCT(mel, cepstrum[n]);                                   //DCT


		for (int j = 0; j < 13; j++)                             //写入mel cepstrum
		{
			outfile1 << cepstrum[n][j] << " ";
		}
		outfile1 << endl;
		

	}
	outfile1.close();

	mfcc39.resize(num - 4);
	for (int i = 2; i < num - 2; i++)
	{
		mfcc39[i - 2].resize(39);
		for (int j = 0; j < 13; j++)
		{
			mfcc39[i - 2][j] = cepstrum[i][j];
			mfcc39[i - 2][j + 13] = cepstrum[i + 1][j] - cepstrum[i - 1][j];
			mfcc39[i - 2][j + 26] = cepstrum[i + 2][j] - cepstrum[i - 2][j];
		}
	}

	ofstream outfile2;
	outfile2.open("mfcc feature without normilization.txt");
	for (int i = 0; i < num - 4; i++)
	{
		for (int j = 0; j < 39; j++)
		{
			outfile2 << mfcc39[i][j] << " ";
		}
		outfile2 << endl;
	}
	outfile2.close();



	//normalizition
	//mean subtraction
	vector<float> average;
	for (int i = 0; i < 39; i++)
	{
		float total = 0;
		for (int j = 0; j < num - 4; j++)
		{
			total += mfcc39[j][i];
		}
		average.push_back(total / (num - 4));
	}
	for (int i = 0; i < 39; i++)
	{
		for (int j = 0; j < num - 4; j++)
		{
			mfcc39[j][i] = mfcc39[j][i] - average[i];
		}
	}

	//variance normalization
	vector<float> variance;
	for (int i = 0; i < 39; i++)
	{
		float total = 0;
		for (int j = 0; j < num - 4; j++)
		{
			total += mfcc39[j][i] * mfcc39[j][i];
		}
		variance.push_back(sqrt(total / (num - 4)));
	}
	for (int i = 0; i < 39; i++)
	{
		for (int j = 0; j < num - 4; j++)
		{
			mfcc39[j][i] = mfcc39[j][i] / variance[i];
		}
	}


}
