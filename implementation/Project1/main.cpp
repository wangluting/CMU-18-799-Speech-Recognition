#include "portaudio.h"
#include "readwave.h"
#include "mfcc.h"
#include "fftw3.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>

using namespace std;

#define SAMPLE_RATE 16000
#define NUM_FRAMES 10000000
#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 1


int j = 0;   //输出数组大小 开始时留出250ms空白
int flag = 0;  //回调函数返回pacomplete则置1
short *recordedSamples = new short[NUM_FRAMES];




struct padata
{
	int num;
	float level;
	float background;
	int nospeak;
};

int EnergyPerFrameInDecibel(short *in, int framesToCalc)
{
	float current = 0;
	for (int i = 0; i < framesToCalc; i++)
	{
		current += (*in + i)*(*in + i);
	}
	current = 10 * log(current);
	return current;
}

static int paCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void *userData)
{

	padata *data = (padata*)userData;
	data->num++;
	short *in = (short*)inputBuffer;
	int framesToCalc = framesPerBuffer;
	float current;
	int finished;


	for (int i = 0; i < framesToCalc; i++)
	{
	recordedSamples[j] = *(in + i);
	j++;
	}

	if (data->num < 5)
	{
		data->background += EnergyPerFrameInDecibel(in, framesToCalc);
		return paContinue;
	}
	if (data->num == 5)
	{
		data->background /= 5;
		return paContinue;
	}

	current = EnergyPerFrameInDecibel(in, framesToCalc);
	data->level = ((data->level * 1) + current) / (1 + 1);
	if (current < data->background)
		data->background = current;
	else
		data->background += (current - data->background)*0.05;
	if (data->level < data->background)
		data->level = data->background;
	printf("level=%f background=%f level-background=%f\n", data->level, data->background, data->level-data->background);
	if (data->level - data->background < 20)
		data->nospeak++;
	else
		data->nospeak = 0;

	printf("%d\n", data->nospeak);

	if (data->nospeak >= 10)
	{
		finished = paComplete;
		flag = 1;
	}
	else
		finished = paContinue;

	return finished;

}



int main()
{
	//设置回调函数参数
	PaStreamParameters inputParameters;
	PaStream *stream;
	PaError err;
	padata data;
	data.num = data.level = data.background = data.nospeak = 0;

	/*for (int i = 0; i < 4000; i++)
	{
		recordedSamples[i] = 0;
	}*/


	ofstream outfile1;
	outfile1.open("power spectrum.txt");
	ofstream outfile2;
	outfile2.open("log mel energy before DCT.txt");
	ofstream outfile3;
	outfile3.open("mel cepstrum.txt");
	ofstream outfile4;
	outfile4.open("log mel energy after IDCT.txt");
	ofstream sampleoutfile;

	//Portaudio初始化
	err = Pa_Initialize();
	if (err != paNoError) goto error;
	//设置参数
	inputParameters.device = Pa_GetDefaultInputDevice();    /* default input device */
	if (inputParameters.device == paNoDevice) goto error;
	inputParameters.channelCount = 1;                       /* mono channel input */
	inputParameters.sampleFormat = paInt16;               /* 16 bit floating point input */
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	//打开输入流
	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		NULL,
		SAMPLE_RATE,						/* Samplerate in Hertz. */
		FRAMES_PER_BUFFER,                 /* Small buffers */
		paClipOff,
		paCallback,
		&data);
	if (err != paNoError) goto error;

	//等待键盘输入
	cout << "Please press any key to begin." << endl; fflush(stdout);
	getc(stdin);

	//开始调用回调函数
	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	//休眠250ms
	//Pa_Sleep(250);

	//等待回调函数返回
	while (!flag);

	//关闭流
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	int numSamples = j + 1;
	int sampleRate = SAMPLE_RATE;
	

	
	sampleoutfile.open("capturedaudio.txt");
	for (int i = 0; i < j; i++)
	{
		sampleoutfile << recordedSamples[i] << " ";
	}
	sampleoutfile.close();


	cout << "Saving the audio..." << endl;
	WriteWave("capturedaudio.wav", recordedSamples, numSamples, SAMPLE_RATE);
	cout << "Completed..." << endl;
	
	//delete recordedSamples;

	//读取
	short* sample = ReadWave("capturedaudio.wav", &numSamples,  &sampleRate);

	//预加重
	float* preSample = new float[numSamples];
	preEmphasize(preSample, sample, numSamples, 0.95);


	//分帧
	const int numFrame = 400;
	const int notOverLap = 200;
	float hamingwindow[numFrame]; //构造汉明窗
	for (int i = 0; i < numFrame; i++)
	{
		hamingwindow[i] = 0.54 - 0.46*cos(2 * 3.14*i / numFrame);
	}


	int num = (numSamples - numFrame) / notOverLap + 1;
	for (int n = 0; n < num; n++)
	{
		
		float windowSample[512];    //加窗后的采样点
		float energySpectrum[257];  //DFT后的频谱
		//加窗
		int i;
		for (i = 0; i < numFrame && (i+n*notOverLap)<numSamples; i++)
		{
			windowSample[i] = preSample[i + n*notOverLap] * (hamingwindow[i]);
		}
	
	
		//补零
		
		for (int k = i; k < 512; k++)
		{
			windowSample[k] = 0;
			//cout << windowSample[k] << " ";
		}
		
		//delete preSample;
		
		//离散傅里叶变换
		FFT(windowSample, energySpectrum);


		//写入频谱
		for (int j = 0; j < 257; j++)
		{
			outfile1 << energySpectrum[j] << " ";
		}
		outfile1 << endl;


		//计算mel spectrum
		float mel[NUM_FILTER];
		memset(mel, 0, sizeof(float)*NUM_FILTER);
		computeMel(mel, sampleRate, energySpectrum);
	
		//转换成 log mel spectrum
		for (int j = 0; j < NUM_FILTER; j++)
		{
			if(mel[j] <= -0.0001 || mel[j] >= 0.0001)
			mel[j] = log(mel[j]);
		}

		//写入log mel before DCT
		for (int j = 0; j <  NUM_FILTER; j++)
		{
			outfile2 << mel[j] << " ";
		}
		outfile2 << endl;


		//DCT	
		float cepstrum[13];
		memset(cepstrum, 0, sizeof(float)*13);
		DCT(mel, cepstrum);
	
		//写入mel cepstrum
		for (int j = 0; j < 13; j++)
		{
			outfile3 << cepstrum[j] << " ";
		}
		outfile3 << endl;

		//IDCT
		//补零64bit
		double cepstrum_zeropad[64];
		double remel[NUM_FILTER];
		for (int j = 0; j < 64; j++)
		{
			if (j < 13)
			{
				cepstrum_zeropad[j] = cepstrum[j];
			}
			else
				cepstrum_zeropad[j] = 0;
		}
		IDCT(cepstrum_zeropad, remel);

		//写入log mel energy after IDCT
		for (int j = 0; j < NUM_FILTER; j++)
		{
			outfile4 << remel[j] << " ";
		}
		outfile4 << endl;

	}
	outfile1.close();
	outfile2.close();
	outfile3.close();
	outfile4.close();
	while (1);
error:
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return -1;

}