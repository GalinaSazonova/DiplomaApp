#ifndef SPLINEWAVELETANALISYS_H
#define SPLINEWAVELETANALISYS_H

#include "wavClass.h"
#include "adaptiveClass.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <math.h>

class SplineWaveletAnalysis
{
public:
    SplineWaveletAnalysis(wavFileParse wavFile);
    //SplineWaveletAnalisys(fil);
    ~SplineWaveletAnalysis();

    double getEpsMin();
    double getEpsMax();
    double getCurrentEps();
    //true - if new value exepted
    bool setCurrentEps(double _eps);
    long int getInitFlowSize();
    int getAdNetSize();

    //build classic adaptive net
    void buildAdaptiveNet();
    //restores initial flow by adaptive net only
    //_return flow
    short *restoreByAdaptiveNet();
    //builds wave array
    void buildWaveFlow();
    //restores initial flow by adaptive net and wave flow
    //_return flow
    short *restoreByAdNetAndWaveFlow();
    //return % of coincedence
    double compareTwoFlows(short *f1, short *f2, long int size1, long int size2);

    //builds int wave array
    void buildIntWaveFlow();
    //restores initial flow by adaptive net and wave flow using int calculations
    //_return flow
    short *restoreIntByAdNetAndWaveFlow();

    void compareTwoNets();

    //save data in wavFile
    bool saveWavFile(short *data, long int size, wav_hdr wavHeader, const char *filename);
    //save data in txtFile
    bool saveTxtFile(short *data, int *index, long int size, wav_hdr wavHeader, char *filename);


private:

    //methods
    short maxDelta();
    int findKappaIndex(int q);

    //var
    wavFileParse w;
    adaptiveNet *adn;

    //size of data chunk
    long int wsize;
    //size of adaptive net
    int adnetsize;
    double eps;
    //minimum value for eps
    double epsMin;
    //max value for eps
    double epsMax;
    //really its 1/h -- step
    double h;

    //initial flow
    short *mainflow;
    //array with values in adaptive net
    short *adnetres;
    //array with adaptive net indexes
    int *adnetindex;
    //array with builded wave flow values
    double *waveflow;
    //array with builded wave flow values (int variant, in case is mult to (coef))
    short *integerwaveflow;
    //data restored
    short *flowrestored;


};

#endif // SPLINEWAVELETANALISYS_H
