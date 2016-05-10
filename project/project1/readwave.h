#ifndef _READWAV_H_
#include "stdio.h"

struct WavFileHead
{
	//Resource Interchange File Flag (0-3) "RIFF"
	char RIFF[4];
	//File Length ( not include 8 bytes from the beginning ) (4-7)
	int FileLength;
	//WAVE File Flag (8-15) "WAVEfmt "
	char WAVEfmt_[8];
	//Transitory Byte ( normally it is 10H 00H 00H 00H ) (16-19)
	unsigned int noUse;
	//Format Category ( normally it is 1 means PCM-u Law ) (20-21)
	short FormatCategory;
	//NChannels (22-23)
	short NChannels;
	//Sample Rate (24-27)
	int SampleRate;
	//l=NChannels*SampleRate*NBitsPersample/8 (28-31)
	int SampleBytes;
	//i=NChannels*NBitsPersample/8 (32-33)
	short BytesPerSample;
	//NBitsPersample (34-35)
	short NBitsPersample;
	//Data Flag (36-39) "data"
	char data[4];
	//Raw Data File Length (40-43)
	int RawDataFileLength;
}__attribute((packed));

// original functions
bool	WaveRewind(FILE *wav_file, WavFileHead *wavFileHead);
short	*ReadWave(const char *wavFile, int *numSamples, int *sampleRate);
void	WriteWave(const char *wavFile, short *waveData, int numSamples, int sampleRate);
void	FillWaveHeader(void *buffer, int raw_wave_len, int sampleRate);

// additive functions
void    GetWavHeader(const char *wavFile,short *Bits,int *Rate,short *Format,int *Length,short *Channels);
short   *ReadWavFile(const char *wavFile, int *numSamples, int *sampleRate);
void    readwav_t(const char *wavFile, short *waveData, long times, int *numSamples, int *sampleRate);
void    GetWavTime(const char *wavFile, double *duration);
void    ReadWav(const char *wavFile, short *waveData, int *numSamples, int *sampleRate);

#endif //_READWAV_H_
