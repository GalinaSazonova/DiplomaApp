#include "wavClass.h"

wavFileParse::wavFileParse()
{

}

wavFileParse::~wavFileParse()
{
	delete[] buffer;
	delete[] gWavDataIn;
}

//Returns size of file
int wavFileParse::getFileSize(FILE* inFile)
{
    uint32_t fileSize = 0;
	fseek(inFile, 0, SEEK_END);

	fileSize = ftell(inFile);

	fseek(inFile, 0, SEEK_SET);
	return fileSize;
}

//Parsing wav file
void wavFileParse::readWav(const char* filename)
{
    fileName = filename;
    uint32_t headerSize = sizeof(wav_hdr);
	filelength = 0;
    FILE* wavFile;
    fopen_s(&wavFile, filename, "rb");

	if (wavFile == nullptr)
	{
       // QDebug() << (stderr, "Unable to open wave file: %s\n", filename);
		return;
	}

	//Read the header
	size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile);
	//displayInf();
	if (bytesRead > 0)
	{
		//Read the data
		bytesPerSample = wavHeader.bitsPerSample / 8;      //Number of bytes per sample
		numSamples = wavHeader.ChunkSize / bytesPerSample; //How many samples are in the wav file?
        static const uint32_t BUFFER_SIZE = wavHeader.Subchunk2Size;
		//cout << BUFFER_SIZE << endl;
		//Here real data read!!!!!!!!!!!!!
		maxInSamples = wavHeader.Subchunk2Size;
		maxInSamples /= wavHeader.bitsPerSample / 8;
        //cout << maxInSamples << endl;
		//cout << maxInSamples << endl;
		buffer = new short[BUFFER_SIZE];
		bytesRead = fread((void*)buffer, BUFFER_SIZE, (size_t)1, wavFile);
		//cout << "Read " << bytesRead << " bytes." << endl;
		
		//cout << "kek" <<  buffer[0] << endl;
        //cout << buffer[maxInSamples - 1] << endl;
		//cout << buffer[maxInSamples + 1] << endl;
		//pU = (short*) buffer;
		
		gWavDataIn = new double[maxInSamples];
		for (int i = 0; i < maxInSamples; i++)
		{
			gWavDataIn[i] = (double) (buffer[i]);
		}
		/*cout << "dou" << gWavDataIn[0] << endl;
		cout << gWavDataIn[23] << endl;
		cout << gWavDataIn[2] << endl;
		cout << gWavDataIn[3] << endl;
		cout << gWavDataIn[4] << endl;
		cout << gWavDataIn[5] << endl;
		cout << gWavDataIn[maxInSamples - 1] << endl;*/
		//delete[] buffer;
	}
	filelength = getFileSize(wavFile);
	fclose(wavFile);
}

void wavFileParse::displayInf()
{
    //qDebug() << "File is                    :" << filelength << " bytes.";
	cout << "RIFF header                :" << wavHeader.RIFF[0] << wavHeader.RIFF[1] << wavHeader.RIFF[2] << wavHeader.RIFF[3] << endl;
	cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
	cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;
	cout << "Data size                  :" << wavHeader.ChunkSize << endl;

	// Display the sampling Rate from the header
	cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
	cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
	cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
	cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
	cout << "Data length                :" << wavHeader.Subchunk2Size << endl;
	cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
	// Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	cout << "Block align                :" << wavHeader.blockAlign << endl;
	cout << "Data string                :" << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
}

const char* wavFileParse::getFileName()
{
    return fileName;
}

//Saving wav data in new file
void wavFileParse::saveWav(char *filename)
{
	FILE* pFile;
	pFile = fopen(filename, "wb");


	fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
	fwrite(buffer, maxInSamples, 2, pFile);
	//fwrite(buffer, wavHeader.Subchunk2Size, 1, pFile);
	
	fclose(pFile);
}


void wavFileParse::saveWav1(char *filename, short* data)
{
	FILE* pFile;
	pFile = fopen(filename, "wb");


	fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
	fwrite(data, maxInSamples, 2, pFile);
	//fwrite(buffer, wavHeader.Subchunk2Size, 1, pFile);

	fclose(pFile);
}

//Returns wav data in double format
double* wavFileParse::wavData()
{
	return gWavDataIn;
}

void wavFileParse::saveW(short *data, int *data2, int adsize, char *filename)
{
	FILE* pFile;
	pFile = fopen(filename, "wb");


	fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
	fwrite(data, adsize, 2, pFile);
	fwrite(data2, adsize, 4, pFile);
	//fwrite(buffer, wavHeader.Subchunk2Size, 1, pFile);

	fclose(pFile);
}


//Returns data in short format
short int* wavFileParse::wavData2()
{
	return buffer;
}


//Returns size of data array
long int wavFileParse::maxInS()
{
	return maxInSamples;
}

//Returns number of bits per sample
uint16_t wavFileParse::bitsPerSam()
{
	return wavHeader.bitsPerSample;
}

//Returns sampling frequency in Hz
uint32_t wavFileParse::SamplesPerS()
{
	return wavHeader.SamplesPerSec;
}

uint16_t wavFileParse::numOfchan()
{
	return wavHeader.NumOfChan;
}

wav_hdr wavFileParse::getWavHeader()
{
    return wavHeader;
}
