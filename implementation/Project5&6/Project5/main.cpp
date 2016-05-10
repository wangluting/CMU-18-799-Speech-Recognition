#include "portaudio.h"
#include "readwave.h"
#include "mfcc.h"
#include "fftw3.h"
#include "SegmentalKmeans.h"
#include "SegmentalKmeansGMM.h"
#include "HMMModel.h";
#include "HMMModelGMM.h"
#include "HMMModelSGM.h"
#include "create_trellis.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <sstream>
#include <time.h>

using namespace std;

#define SAMPLE_RATE 16000
#define NUM_FRAMES 10000000
#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 1
#define FEATURE_LENGTH 39
#define KERNEL_NUMBER 4


vector<vector<TreeMap> > firstpath_tree_Map(FIRSTPATHSTATE);
vector<vector<TreeMap> > secondpath_tree_Map(SECONDPATHSTATE);
vector<vector<Trellis> > trellis;
vector<int> firstpath_lm_state_index;
vector<int> secondpath_lm_state_index;
vector<HMMModel*> models;
stack<int> result;

int flag = 0;  //回调函数返回pacomplete则置1
vector<short> recordedSamples;             //the captured audio
int end_Threshold = 20;
int start_Threshold = 20;
int threshold = start_Threshold;
bool start = false;
struct padata
{
	int num;
	float level;
	float background;
	int nospeak;
	int speak;
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

	for (int i = 0; i < framesToCalc; i++)
	{
		recordedSamples.push_back(*(in + i));
	}
	current = EnergyPerFrameInDecibel(in, framesToCalc);
	data->level = ((data->level * 1) + current) / (1 + 1);
	if (current < data->background)
		data->background = current;
	else
		data->background += (current - data->background)*0.05;
	if (data->level < data->background)
		data->level = data->background;
	//printf("level=%f background=%f level-background=%f\n", data->level, data->background, data->level-data->background);
	if (data->level - data->background < threshold)
	{
		cout << "silience" << endl;
		data->nospeak++;
		data->speak = 0;

	}
	else
	{
		cout << "speaking" << endl;
		data->speak++;
		data->nospeak = 0;
	}

	//printf("%d\n", data->nospeak);
	if (data->speak >= 5 && start == false)
	{
		threshold = end_Threshold;
		start = true;
	}

	if (data->nospeak >= 10)
	{
		if (start == true)
		{
			finished = paComplete;
			flag = 1;
		}
		else
			finished = paContinue;

	}
	else
		finished = paContinue;

	return finished;

}



int main()
{
	
	PaStreamParameters inputParameters;//设置回调函数参数
	PaStream *stream;
	PaError err;
	padata data;
	data.num = data.level = data.background = data.nospeak = data.speak = 0;
	//Portaudio初始化
	err = Pa_Initialize();
	inputParameters.device = Pa_GetDefaultInputDevice();    // default input device 
	inputParameters.channelCount = 1;                       // mono channel input 
	inputParameters.sampleFormat = paInt16;               // 16 bit floating point input 
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;
	err = Pa_OpenStream(                                   //打开输入流
		&stream,
		&inputParameters,
		NULL,
		SAMPLE_RATE,						// Samplerate in Hertz. 
		FRAMES_PER_BUFFER,                 // Small buffers 
		paClipOff,
		paCallback,
		&data);
	cout << "Please press any key to begin." << endl; fflush(stdout);   	//等待键盘输入
	getc(stdin);
	err = Pa_StartStream(stream);              //开始调用回调函数
	while (!flag);                       //等待回调函数返回
	err = Pa_CloseStream(stream);                 //关闭流



	cout << "Saving the audio..." << endl;             //save audio
	cout << "Please input the filename you want to save the capture audio." <<  endl;
	string filename_audio; cin >> filename_audio;
	string name = filename_audio + ".wav";
	int numSamples = recordedSamples.size();
	int sampleRate = SAMPLE_RATE;
	WriteWave(name.data(), &recordedSamples[0], numSamples, SAMPLE_RATE);
	cout << "The audio has been successfully saved..." << endl;
	

	short* sample = ReadWave(name.data(), &numSamples,  &sampleRate);  //读取
	vector<vector<float> > mfcc39;
	getMfccFeature(sample, mfcc39, numSamples);          //归一化39维MFCC特征
	

	ofstream outfile3;
	name = filename_audio + ".txt";
	outfile3.open(name.data());
	for(int i = 0; i < mfcc39.size(); i++)
	{
		for (int j = 0; j < 39; j++)
			outfile3 << mfcc39[i][j] << " ";
		if(i != (mfcc39.size()-1)) outfile3 << endl;
	}
	outfile3.close();
	cout << "The MFCC feature has been saved!" << endl;

	//while(1);
	


	
	
	cout << "First, we will load the trained GMM!" << endl;
	std::string path = "C:\\Users\\Christina\\Desktop\\new_audio\\";
	std::string classes[11] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "sil"};
	//std::string classes[6] = { "zero", "one", "two", "three", "four", "sil" };
	//std::vector<HMMModel*> models;

	for (int i = 0; i < 11; i++)
	{
		HMMModel * model = new HMMModelGMM();
		model->import(path + classes[i] + "GMM.txt");
		models.push_back(model);
	}
	cout << "Now, we will recognize what you are saying!" << endl;
	creat_Map();

	
	/*
	vector<vector<float> > mfcc39_test;
	ifstream in("C:\\Users\\Christina\\Desktop\\new_audio\\2345678.txt");
	string s;
	float n;
	while (getline(in, s))
	{
		istringstream istr(s);
		vector<float> temp;
		while (!istr.eof())
		{
			istr >> n;
			temp.push_back(n);
		}
		//temp.pop_back();
		mfcc39_test.push_back(temp);
	}
	in.close();
	*/


	time_t start, stop;
	start = time(NULL);
	search(mfcc39);
	//search(mfcc39_test);
	stop = time(NULL);
	cout << "Done!" << endl << "Computation time is:" << stop - start << "s" << endl;

	
	for (int i = 0; i < models.size(); i++)
	{
		delete models[i];
	}
	models.clear();


	while (1);
}
	



	
	
	