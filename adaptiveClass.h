#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <math.h>

#include "wavClass.h"

using namespace std;

class adaptiveNet
{
public:
	adaptiveNet(const char *filename);
	~adaptiveNet();
	void buildGrid();
	void restoreFlow();
	void buildWaveFlow();
	void restoreByWaveFlow();
	void printEps();
	void changeEps(double neweps);
	void printEpsMin();
	void printEpsMax();
	void saveResInWav(char *filename);
	void saveResInWav2(char *filename);
	void compareres();
	double retepsMax();
	void saveNet(char *filename);

    short* getAdNet();
    long int getAdNetSize();

private:
	wavFileParse w;
	long int wsize;
	int adnetsize;
	double eps;
	double epsMin;
	double epsMax;
	short *adnetres;
	int *adnetindex;
	double *waveflow;
	short *flowrestored;
	short *flowrestored2;

	void grid(int iw, int ires, int adnetstop);
	void agrid(int iflow, int iadnet, int adnetstop);
	void gridDiff(double *u, double eps, int R, int K, int *itx);
	void makeWaveflow(int iw, int adnetstop);
	void rebuildFlow(int iw, int adnetstop);
	short maxDelta();
	//void deleteOldResults();
	int findKappaIndex(int q);
};
