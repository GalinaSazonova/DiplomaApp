#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <math.h>

using namespace std;

typedef struct  WAV_HEADER
{
	/* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	uint8_t         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        bytesPerSec;    // bytes per second
	uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bitsPerSample;  // Number of bits per sample
	/* "data" sub-chunk */
	uint8_t         Subchunk2ID[4]; // "data"  string
	uint32_t        Subchunk2Size;  // Sampled data length
} wav_hdr;


class wavFileParse
{
public:
	wavFileParse();
	~wavFileParse();
	int getFileSize(FILE* inFile);
	void readWav(const char *filename);
	void displayInf();
	void saveWav(char *filename);
	void saveWav1(char *filename, short *data);
	double* wavData();
	short int* wavData2();
	long int maxInS();
	uint16_t bitsPerSam();
	uint32_t SamplesPerS();
	uint16_t numOfchan();
	void saveW(short *data, int *data2, int adsize, char *filename);
    const char* getFileName();

    wav_hdr getWavHeader();

private:
	wav_hdr wavHeader;
	uint16_t bytesPerSample;
	uint64_t numSamples;
	double* gWavDataIn;
	
	short* buffer;
	int filelength;
	long int maxInSamples;

    const char* fileName;
};

