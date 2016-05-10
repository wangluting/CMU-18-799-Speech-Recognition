#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include <string.h>
#include "readwave.h"


bool WaveRewind(FILE *wav_file, WavFileHead *wavFileHead)
{
	char riff[8],wavefmt[8];
	short i;
	rewind(wav_file);
	fread(wavFileHead,sizeof(struct WavFileHead),1,wav_file);

	for ( i=0;i<8;i++ )
	{
		riff[i]=wavFileHead->RIFF[i];
		wavefmt[i]=wavFileHead->WAVEfmt_[i];
	}
	riff[4]='\0';
	wavefmt[7]='\0';
	if ( strcmp(riff,"RIFF")==0 && strcmp(wavefmt,"WAVEfmt")==0 )
		return	true;  // It is WAV file.
	else
	{
		rewind(wav_file);
		return(false);
	}
}


short *ReadWave(const char *wavFile, int *numSamples, int *sampleRate ) 
{                                                               
	FILE	*wavFp;
	WavFileHead		wavHead;
	short	*waveData;
	long	numRead;

	wavFp = fopen(wavFile, "rb");
	if (!wavFp)	
	{
		printf("\nERROR:can't open %s!\n", wavFile);
		exit(0);
	}

	if (WaveRewind(wavFp, &wavHead) == false)
	{
		printf("\nERROR:%s is not a Windows wave file!\n", wavFile);
		exit(0);
	}

	waveData = new short [wavHead.RawDataFileLength/sizeof(short)];
	numRead = fread(waveData, sizeof(short), wavHead.RawDataFileLength / 2, wavFp);
	assert(numRead * sizeof(short) == (unsigned long)wavHead.RawDataFileLength);
	fclose(wavFp);

	*numSamples = wavHead.RawDataFileLength/sizeof(short);
	*sampleRate = wavHead.SampleRate;
	return	waveData;
}

void FillWaveHeader(void *buffer, int raw_wave_len, int sampleRate)
{
	WavFileHead  wavHead;

	strcpy(wavHead.RIFF, "RIFF");
	strcpy(wavHead.WAVEfmt_, "WAVEfmt ");
	wavHead.FileLength = raw_wave_len + 36;
	wavHead.noUse = 16;
	wavHead.FormatCategory = 1;
	wavHead.NChannels = 1;
	wavHead.SampleRate = sampleRate;
	wavHead.SampleBytes = sampleRate*2;
	wavHead.BytesPerSample = 2;
	wavHead.NBitsPersample = 16;
	strcpy(wavHead.data, "data");
	wavHead.RawDataFileLength = raw_wave_len;

	memcpy(buffer, &wavHead, sizeof(WavFileHead));
}

void WriteWave(const char *wavFile, short *waveData, int numSamples, int sampleRate)
{
	FILE	*wavFp;
	WavFileHead		wavHead;
	long	numWrite;

	wavFp = fopen(wavFile, "wb");
	if (!wavFp)	
	{
		printf("\nERROR:can't open %s!\n", wavFile);
		exit(0);
	}

	FillWaveHeader(&wavHead, numSamples*sizeof(short), sampleRate);
	fwrite(&wavHead, sizeof(WavFileHead), 1, wavFp);
	numWrite = fwrite(waveData, sizeof(short), numSamples, wavFp);
	assert(numWrite == numSamples);
	fclose(wavFp);
}

void GetWavHeader(const char *wavFile, short *Bits, int *Rate,
				  short *Format, int *Length, short *Channels) 
{                                                               
	FILE	*wavFp;
	WavFileHead		wavHead;
	char    *waveData;
	long	numRead,File_length;

	wavFp = fopen(wavFile, "rb");
	if (!wavFp)	
	{
		printf("\nERROR:can't open %s!\n", wavFile);
		exit(0);
	}
    fseek(wavFp,0,SEEK_END);
	File_length=ftell(wavFp);

	if (WaveRewind(wavFp, &wavHead) == false)
	{
		printf("\nERROR:%s is not a Windows wave file!\n", wavFile);
		exit(0);
	}

	waveData = new char[(File_length-sizeof(struct WavFileHead))/sizeof(char)];
	numRead = fread(waveData, sizeof(char), File_length-sizeof(struct WavFileHead), wavFp);
	fclose(wavFp);

	*Bits = wavHead.NBitsPersample;
	*Format = wavHead.FormatCategory;
	*Rate = wavHead.SampleRate;
	*Length = (int)numRead;
	*Channels = wavHead.NChannels;

	delete []	waveData;
}


short *ReadWavFile(const char *wavFile, int *numSamples, int *sampleRate )
{                                                               
	FILE	*wavFp;
	WavFileHead		wavHead;
	short	*waveData;
	long	numRead,File_length;

	wavFp = fopen(wavFile, "rb");
	if (!wavFp)	
	{
		printf("\nERROR:can't open %s!\n", wavFile);
		exit(0);
	}
    fseek(wavFp,0,SEEK_END);
	File_length=ftell(wavFp);


	if (WaveRewind(wavFp, &wavHead) == false)
	{
		printf("\nERROR:%s is not a Windows wave file!\n", wavFile);
		exit(0);
	}

	waveData = new short [(File_length-sizeof(struct WavFileHead))/sizeof(short)];
	numRead = fread(waveData, sizeof(short), (File_length-sizeof(struct WavFileHead))/sizeof(short), wavFp);
	fclose(wavFp);

	*numSamples = (int)numRead;
	*sampleRate = wavHead.SampleRate;
	return	waveData;
}

void ReadWav(const char *wavFile, short *waveData, int *numSamples, int *sampleRate)
{                                                               
	FILE	*wavFp;
	WavFileHead		wavHead;
	long	numRead;

	wavFp = fopen(wavFile, "rb");
	if (!wavFp)	
	{
		printf("\nERROR:can't open %s!\n", wavFile);
		exit(0);
	}

	if (WaveRewind(wavFp, &wavHead) == false)
	{
		printf("\nERROR:%s is not a Windows PCM file!\n", wavFile);
		exit(0);
	}

	numRead = fread(waveData, sizeof(short), wavHead.RawDataFileLength/2, wavFp);
	assert(numRead*sizeof(short) == (unsigned long)wavHead.RawDataFileLength);
	fclose(wavFp);

	*numSamples = wavHead.RawDataFileLength/sizeof(short);
	*sampleRate = wavHead.SampleRate;
}