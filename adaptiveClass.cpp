#include "adaptiveClass.h"

adaptiveNet::adaptiveNet(const char *filename)
{
	w.readWav(filename);
    w.displayInf();
	wsize = w.maxInS();
	
	adnetres = new short[wsize + 1];
	adnetindex = new int[wsize + 1];
	flowrestored = new short[wsize + 1];
	waveflow = new double[wsize + 1];
	flowrestored2 = new short[wsize + 1];

    short maxDelt = maxDelta();
	//cout << maxDelt << endl;
	epsMin = (1 / (double)w.SamplesPerS()) * (double)maxDelt;
	epsMax = (wsize - 1) / (double)w.SamplesPerS() * (double)maxDelt;
	//eps = 1000;
	eps = epsMax/3;
}

adaptiveNet::~adaptiveNet()
{
	delete[] adnetindex;
	delete[] adnetres;
	delete[] flowrestored;
	delete[] waveflow;
	delete[] flowrestored2;
}

///<summary>
///returns max value of difference of two adjacent numbers in flow
///</summary>
short adaptiveNet::maxDelta()
{
	short max = 0;
	for (long i = 0; i < wsize - 1; i++)
	{
		short temp = abs(w.wavData2()[i + 1] - w.wavData2()[i]);
		if (temp > max)
		{
			max = temp;
		}

	}
	return max;
}

void adaptiveNet::printEpsMax()
{
	cout << "Max epsilon = " << epsMax << endl;
}

void adaptiveNet::printEpsMin()
{
	cout << "Min epsilon = " << epsMin << endl;
}

void adaptiveNet::printEps()
{
	cout << "Current epsilon = " << eps << endl;
}


///saves restored with adaptive net
void adaptiveNet::saveResInWav(char *filename)
{
	w.saveWav1(filename, flowrestored);
}

///saves restored by reconstruction formulas
void adaptiveNet::saveResInWav2(char *filename)
{
	w.saveWav1(filename, flowrestored2);
}

///<summary>
///changes epsilon
///</summary>
void adaptiveNet::changeEps(double neweps)
{
	if (neweps < epsMin || neweps > epsMax)
	{
		cout << "Wrong epsilon" << endl;
		return;
	}
	eps = neweps;
}

double adaptiveNet::retepsMax()
{
	return epsMax;
}

/// <summary>
/// builds adaptive net
/// </summary>
/// <param name="iw">index of u to start</param>
/// <param name="ires">index of res to start</param>
void adaptiveNet::grid(int iw, int ires, int adnetstop)
{
	adnetres[ires] = w.wavData2()[iw];
	adnetindex[ires] = iw;
	int prev = iw;
	int current = iw + 1;
	ires++;
	while (current != adnetstop)
	{
		short delta = abs(w.wavData2()[current] - w.wavData2()[prev]);
		
		if (delta <= eps)
		{
			current++;
			continue;
		}
		else
		{
			if (adnetres[ires - 1] != w.wavData2()[current - 1])
			{
				adnetres[ires] = w.wavData2()[current - 1];
				prev = current - 1;
				adnetindex[ires] = current - 1;
				ires++;
			}
			else
			{
				adnetres[ires] = w.wavData2()[current];
				prev = current;
				adnetindex[ires] = current;
				current++;
				ires++;
			}
		}
	}
	if (adnetres[ires - 1] != w.wavData2()[adnetstop - 1])
	{
		adnetres[ires] = w.wavData2()[adnetstop - 1];
		adnetindex[ires] = adnetstop - 1;
		ires++;
	}
	adnetsize = ires;
}

/// <summary>
/// rebuild data by adaptive net
/// </summary>
/// <param name="adnetstop">size of adnet or midres</param>
/// <param name="iflow">index in res to start</param>
/// <param name="iaadnet">index in adnet to start</param>
void adaptiveNet::agrid(int iflow, int iadnet, int adnetstop)
{
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

		double k = (double)(adnetres[iadnet + 1] - adnetres[iadnet]) / (adnetindex[iadnet + 1] - adnetindex[iadnet]);
		for (int i = 1; i <= delta; i++)
		{
			flowrestored[iflow] = (short)round(i * k + (double)adnetres[iadnet]);
			iflow++;
		}
		iadnet++;
	}
}

int adaptiveNet::findKappaIndex(int q)
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

void adaptiveNet::makeWaveflow(int iw, int adnetstop)
{
	int i = 0;
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
		waveflow[iw] = (short)round(w.wavData2()[iw] - (double)w.SamplesPerS() / (double)(adnetindex[j + 1] - adnetindex[j])*((double)(adnetindex[j + 1] - iw) / w.SamplesPerS()
			* w.wavData2()[adnetindex[j]] + (double)(iw - adnetindex[j]) / w.SamplesPerS() * w.wavData2()[adnetindex[j+1]]));
		iw++;
	}
}

void adaptiveNet::rebuildFlow(int iw, int adnetstop)
{
	int i = 0;
	while (iw != adnetstop)
	{
		if (iw == adnetindex[i])
		{
			flowrestored2[iw] = adnetres[i];
			iw++;
			i++;
			continue;
		}
		int j = findKappaIndex(iw);
		flowrestored2[iw] = (short)round(waveflow[iw] + (double)w.SamplesPerS() / (double)(adnetindex[j + 1] - adnetindex[j])*((double)(adnetindex[j + 1] - iw) / w.SamplesPerS()
			* adnetres[j] + (double)(iw - adnetindex[j]) / w.SamplesPerS() * adnetres[j + 1]));
		iw++;
	}
}

void adaptiveNet::buildGrid()
{
	grid(0, 0, wsize);
	cout << "and size = "<< adnetsize << endl;
}

void adaptiveNet::restoreFlow()
{	
	agrid(0, 0, adnetsize);	
}

void adaptiveNet::buildWaveFlow()
{
	makeWaveflow(0, wsize);
}

void adaptiveNet::restoreByWaveFlow()
{
	rebuildFlow(0, wsize);
}


void adaptiveNet::compareres()
{
	int i = 0;
	int adcoin = 0;
    long int wavcoin = 0;
	while (i != wsize)
	{
		if (flowrestored[i] == w.wavData2()[i])
		{
			adcoin++;
		}
		if (flowrestored2[i] == w.wavData2()[i])
		{
			wavcoin++;
		}
		i++;
	}
	//cout << adcoin << endl;
	cout << wavcoin << endl;
	//cout << " adnet coinc = " << (double)adcoin / wsize * 100 << endl;
	cout << "wave coinc = " << (double)wavcoin / wsize * 100 << endl;
}

short* adaptiveNet::getAdNet()
{
    return adnetres;
}

long int adaptiveNet::getAdNetSize()
{
    return adnetsize;
}


void adaptiveNet::saveNet(char* filename)
{
	//FILE* pFile;
	//pFile = fopen(filename, "wb");
	
	//fwrite(&wavHeader, sizeof(WAV_HEADER), 1, pFile);
	//fwrite(adnetres, adnetsize,2, pFile);
	//fwrite((const char*)"1", sizeof(char), 2, pFile);
	//fwrite(adnetindex, adnetsize, sizeof(int), pFile);
	//fclose(pFile);

	w.saveW(adnetres, adnetindex, adnetsize, filename);
}

/// <summary>
/// adaptive net using regular net, f(t) -- diff
/// </summary>
void adaptiveNet::gridDiff(double *u, double eps, int R, int K, int *itx)
{
	int j0 = 0;
	int j1 = j0 + 1;
	int s = 0;
	itx[0] = 0;
	double du = 0;
	double maxD1u = 0;
	for (int j = 1; j < R; j++)
	{
		du = sqrt(abs(u[j + 1] - 2 * u[j] + u[j - 1]));
		if (maxD1u < du)
		{
			maxD1u = du;
		}
	}
	double epsz = maxD1u;
	double epszz = (R + 1) * epsz;
	if ((eps <= epsz) || (eps > epszz))
	{
		cout << "eps <= epsz or eps > epszz" << endl;
		return;
	}
me:
	if (s > K)
	{
		cout << "s > K" << endl;
		return;
	}
	j1 = j0 + 1;
	maxD1u = sqrt(abs(u[j1] - 2 * u[j0] + u[j0 - 1]));
met:
	if (j1 > R)
	{
		K = s;
		itx[K + 1] = R + 1;
	}
	else
	{
		du = sqrt(abs(u[j1] - 2 * u[j0] + u[j0 - 1]));
		if (maxD1u < du)
			maxD1u = du;
		if (maxD1u*(j1 - j0) <= eps)
		{
			j1++;
			goto met;
		}
		else
		{
			s++;
			itx[s] = j1;
			j0 = j1;
			goto me;
		}
	}
}

