#include "splinewaveletanalysis.h"


SplineWaveletAnalysis::SplineWaveletAnalysis(wavFileParse wavFile)
{
    w = wavFile;
    adn = new adaptiveNet(w.getFileName());
    wsize = w.maxInS();
    h = double(w.SamplesPerS());

    mainflow = w.wavData2();
    //mainflow = new short[wsize + 1];
    adnetres = new short[wsize + 1];
    adnetindex = new int[wsize + 1];
    flowrestored = new short[wsize + 1];
    waveflow = new double[wsize + 1];
    integerwaveflow = new short[wsize + 1];
    //flowrestored2 = new short[wsize + 1];

    short maxDelt = maxDelta();
    //cout << maxDelt << endl;
    epsMin = (1 / h) * 1.0 * maxDelt;
    epsMax = (wsize - 1) / h * 1.0 * maxDelt;
    //eps = 1000;
    eps = epsMax/5;
}

SplineWaveletAnalysis::~SplineWaveletAnalysis()
{
    delete[] mainflow;
    delete[] adnetindex;
    delete[] adnetres;
    delete[] flowrestored;
    delete[] waveflow;
    delete[] integerwaveflow;
    //delete[] flowrestored2;
}

///<summary>
///returns max value of difference of two adjacent numbers in flow
///</summary>
short SplineWaveletAnalysis::maxDelta()
{
    short max = 0;
    for (long i = 0; i < wsize - 1; i++)
    {
        short temp = mainflow[i + 1] - mainflow[i];
        temp = temp < 0 ? -temp : temp;
        if (temp > max)
        {
            max = temp;
        }

    }
    return max;
}

double SplineWaveletAnalysis::getEpsMax()
{
    return epsMax;
}

double SplineWaveletAnalysis::getEpsMin()
{
    return epsMin;
}

double SplineWaveletAnalysis::getCurrentEps()
{
    return eps;
}

bool SplineWaveletAnalysis::setCurrentEps(double _eps)
{
    if (_eps < epsMin || _eps > epsMax)
    {
        return false;
    }
    eps = _eps;
    return  true;
}

long SplineWaveletAnalysis::getInitFlowSize()
{
    return  wsize;
}

int SplineWaveletAnalysis::getAdNetSize()
{
    return adnetsize;
}

void SplineWaveletAnalysis::buildAdaptiveNet()
{
    int ires = 0;
    int iw = 0;
    int adnetstop = wsize;

    adnetres[ires] = mainflow[iw];
    adnetindex[ires] = iw;
    int prev = iw;
    int current = iw + 1;
    ires++;
    while (current != adnetstop)
    {
        short delta = abs(mainflow[current] - mainflow[prev]);

        if (delta <= eps)
        {
            current++;
            continue;
        }
        else
        {
            if (adnetres[ires - 1] != mainflow[current - 1])
            {
                adnetres[ires] = mainflow[current - 1];
                prev = current - 1;
                adnetindex[ires] = current - 1;
                ires++;
            }
            else
            {
                adnetres[ires] = mainflow[current];
                prev = current;
                adnetindex[ires] = current;
                current++;
                ires++;
            }
        }
    }
    if (adnetres[ires - 1] != mainflow[adnetstop - 1])
    {
        adnetres[ires] = mainflow[adnetstop - 1];
        adnetindex[ires] = adnetstop - 1;
        ires++;
    }
    adnetsize = ires;
}

void SplineWaveletAnalysis::compareTwoNets()
{
    adn->buildGrid();

    cout << compareTwoFlows(adnetres, adn->getAdNet() , adnetsize, adn->getAdNetSize()) << endl;
}

short* SplineWaveletAnalysis::restoreByAdaptiveNet()
{
    int iflow = 0;
    int iadnet = 0;
    int adnetstop = adnetsize;

    delete[] flowrestored;
    flowrestored = new short[wsize + 1];

    flowrestored[iflow] = adnetres[iadnet];
    //iadnet++;
    iflow++;
    while (iadnet != adnetstop - 1)
    {
        short delta = abs(adnetindex[iadnet + 1] - adnetindex[iadnet]);
        if (delta == 1)
        {
            iadnet++;
            flowrestored[iflow] = adnetres[iadnet];
            iflow++;
            continue;
        }

        double k = (double)(adnetres[iadnet + 1] - adnetres[iadnet])
                / (adnetindex[iadnet + 1] - adnetindex[iadnet]);
        for (int i = 1; i <= delta; i++)
        {
            flowrestored[iflow] = (short)round(i * k + 1.0 * adnetres[iadnet]);
            iflow++;
        }
        iadnet++;
    }

    return flowrestored;
}

int SplineWaveletAnalysis::findKappaIndex(int q)
{
    int i = 0;
    while (i != adnetsize)
    {
        if (q > adnetindex[i] && q < adnetindex[i + 1])
        {
            return i;
        }
        i++;
    }
    return -1;
}

void SplineWaveletAnalysis::buildWaveFlow()
{
    int iw = 0;
    int i = 0;
    int adnetstop = wsize;

    //delete []waveflow;
    //waveflow = new double[wsize + 1];

    while (iw != adnetstop)
    {
        if (iw == adnetindex[i])
        {
            waveflow[iw] = 0;
            iw++;
            i++;
            continue;
        }
        int j = findKappaIndex(iw);
//        waveflow[iw] = short(round(mainflow[iw] -
//            h / 1.0 * (adnetindex[j + 1] - adnetindex[j])*(1.0 * (adnetindex[j + 1] - iw) / h
//            * mainflow[adnetindex[j]] + 1.0 * (iw - adnetindex[j]) / h * mainflow[adnetindex[j+1]])));
        waveflow[iw] = (short)round(w.wavData2()[iw] - (double)w.SamplesPerS() / (double)(adnetindex[j + 1] - adnetindex[j])*((double)(adnetindex[j + 1] - iw) / w.SamplesPerS()
            * w.wavData2()[adnetindex[j]] + (double)(iw - adnetindex[j]) / w.SamplesPerS() * w.wavData2()[adnetindex[j+1]]));


        iw++;
    }
}

short* SplineWaveletAnalysis::restoreByAdNetAndWaveFlow()
{
    int iw = 0;
    int i = 0;
    int adnetstop = wsize;

    //delete[] flowrestored;
    //flowrestored = new short[wsize + 1];

    while (iw != adnetstop)
    {
        if (iw == adnetindex[i])
        {
            flowrestored[iw] = adnetres[i];
            iw++;
            i++;
            continue;
        }
        int j = findKappaIndex(iw);
//        flowrestored[iw] = short(round(waveflow[iw] + h /
//            1.0 * (adnetindex[j + 1] - adnetindex[j])*(1.0 * (adnetindex[j + 1] - iw) / h
//            * adnetres[j] + 1.0 * (iw - adnetindex[j]) / h * adnetres[j + 1])));
        flowrestored[iw] = (short)round(waveflow[iw] + (double)w.SamplesPerS() / (double)(adnetindex[j + 1] - adnetindex[j])*((double)(adnetindex[j + 1] - iw) / w.SamplesPerS()
                * adnetres[j] + (double)(iw - adnetindex[j]) / w.SamplesPerS() * adnetres[j + 1]));

        iw++;
    }
    return  flowrestored;
}

double SplineWaveletAnalysis::compareTwoFlows(short *f1, short *f2, long int size1, long int size2)
{
    int i = 0;
    long int coin = 0;
    long int size = size1 == size2 ? size1 : 0;
    while (i != size)
    {
        if (f1[i] == f2[i]) //|| (f2[i] - 1 <= f1[i] <= f2[i] + 1) )
        {
            coin++;
        }
        i++;
    }
    return  double(coin) / size * 100;
}

void SplineWaveletAnalysis::buildIntWaveFlow()
{
    int iw = 0;
    int i = 0;
    int adnetstop = wsize;

    //delete []integerwaveflow;
    //integerwaveflow = new short[wsize + 1];

    while (iw != adnetstop)
    {
        if (iw == adnetindex[i])
        {
            integerwaveflow[iw] = 0;
            iw++;
            i++;
            continue;
        }
        int j = findKappaIndex(iw);
        integerwaveflow[iw] = (adnetindex[j+1] - adnetindex[j]) * mainflow[iw] - (adnetindex[j+1] - iw) * mainflow[adnetindex[j]] - (iw - adnetindex[j]) * mainflow[adnetindex[j+1]]; //result (adnetindex[j+1] - adnetindex[j]) * waveflow[iw]
        iw++;
    }
}

short* SplineWaveletAnalysis::restoreIntByAdNetAndWaveFlow()
{
    int iw = 0;
    int i = 0;
    int adnetstop = wsize;

    //delete[] flowrestored;
    //flowrestored = new short[wsize + 1];

    while (iw != adnetstop)
    {
        if (iw == adnetindex[i])
        {
            flowrestored[iw] = adnetres[i];
            iw++;
            i++;
            continue;
        }
        int j = findKappaIndex(iw);
        flowrestored[iw] = (adnetindex[j + 1] - adnetindex[j]) * integerwaveflow[iw] +
                (adnetindex[j + 1] - iw) * adnetres[j] + (iw - adnetindex[j]) * adnetres[j + 1];
        flowrestored[iw] = short(round(flowrestored[iw] * 1.0 / ((adnetindex[j + 1] - adnetindex[j]))));
        iw++;
    }
    return flowrestored;
}

bool SplineWaveletAnalysis::saveWavFile(short *data, long size, wav_hdr wavHeader, const char *filename)
{
    //bool saved = true;
    FILE* pFile;
    fopen_s(&pFile, filename, "wb");

    if (pFile == nullptr)
        return false;

    fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
    fwrite(mainflow, size, 2, pFile);

    fclose(pFile);

    return true;
}

bool SplineWaveletAnalysis::saveTxtFile(short *data, int *index, long size, wav_hdr wavHeader, char *filename)
{
    FILE* pFile;
    fopen_s(&pFile, filename, "wb");

    if (pFile == nullptr)
        return false;
    fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
    fwrite(data, size, 2, pFile);
    fwrite(index, size, 4, pFile);

    fclose(pFile);
    return true;
}
