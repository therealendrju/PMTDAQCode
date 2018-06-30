#include "iostream"
#include "stdlib.h"
#include "math.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>

#include "tools.h"
//extern my_event the_event;

float x_pmt[MY_MAX_NPMT];
float y_pmt[MY_MAX_NPMT];

using namespace std;

void Smooth(float pulse[],int nsamples,int smoothingStep)
{
  vector<float>* backupPulse = new vector<float>;
  
  for (int i=0;i<nsamples;i++)
    {
      backupPulse->push_back(pulse[i]);
    }

  for (int i=smoothingStep;i<nsamples-smoothingStep-1;i++)
    {
      float sum = 0.0;
      for (int j=i-smoothingStep;j<(i+smoothingStep+1);j++)
	sum += (*backupPulse)[j];
      pulse[i] = sum/(2.0*smoothingStep);
    }
  for (int i=0;i<smoothingStep;i++)
    {
      pulse[i] = pulse[smoothingStep];
    }
  for (int i=nsamples-smoothingStep-1;i<nsamples;i++)
    {
      pulse[i] = pulse[nsamples-smoothingStep-2];
    }
  delete backupPulse;
  return;
}

float get_threshold(float pulse[],int nsamples,float samplingfreq,float 
		    pretrigger)
{
   //Compute baseline from 20 ns and for 2 mus
  //cout << "samplingfreq: " << samplingfreq << endl;
  int ifirst = (int) (20./samplingfreq); 
  int ilast = ifirst + (int) ((pretrigger-100.)/samplingfreq)-1; 
  if (ilast <= ifirst)
    {
      cout << "Pretrigger is too short" << endl;
      exit(1);
    }
  //cout << "Pretrigger: " << pretrigger << endl;
  //cout << ifirst << " " << ilast << " " << nsamples << endl;
  float dtres = 0.0015; //it is related to rms of baseline or, better, to the p.e. amplitude
  if (ilast>nsamples) 
    { exit(1);}
  float width = (float) (ilast-ifirst+1);
  float basel=0.0;
 
  for (int i=ifirst;i<ilast;i++)
    basel += pulse[i];
  basel /= width;
  //cout << "Baseline: " << basel << endl;

  //Compute rms_of_baseline
  float rmsbasel=0.0;
  for (int i=ifirst;i<ilast;i++)
    rmsbasel += (pulse[i]-basel)*(pulse[i]-basel);
  rmsbasel /= (float) (ilast-ifirst);
  rmsbasel = sqrt(rmsbasel);
  //cout << "Rms of baseline: " << rmsbasel << endl;
  dtres = 4.0*rmsbasel;
 
  return dtres;

}

bool IsASinglePhotoElectron(float pulse[],int nsamples,float samplingFreq,
			    int candidateChannel,float threshold)
{
  //Check that it is not a single photoelectron.
  //I require at least 3 channels above threshold in 100 ns and at least one channel 
  //over threshold after 100 ns.
  float integral1 = 0;
  int timeWindow = (int)(1000./samplingFreq);
  int timeWindow2 = (int)(100./samplingFreq);
  int chansOverThre1 = 0;
  int chansOverThre2 = 0;
  for (int i=candidateChannel;i < min(candidateChannel+timeWindow,nsamples);i++)
    {
      if (pulse[i] > threshold)
	if (i < (candidateChannel+timeWindow2)) 
	  {
	    integral1 += pulse[i];
	    chansOverThre1++;
	  }
	else 
	  {
	    chansOverThre2++;
	  }
    }

  //  if (chansOverThre1 < 3 || chansOverThre2< 1 || integral1 < (5.0*threshold))
  if (chansOverThre1 < 3 || integral1 < (5.0*threshold))
   {
     //cout << "Looks to be a single p.e. " << candidateChannel*samplingFreq << " " << 
     //	chansOverThre1 << " " << chansOverThre2 << " " << integral1 << endl;
      return true;
    }
  else
    {
      //cout << "Not a single p.e. " << candidateChannel*samplingFreq << " " << 
      //	chansOverThre1 << " " << chansOverThre2 << " " << integral1 << endl;
      return false;
    }
}

bool IsAPrimary(float pulse[],int nsamples,float samplingFreq,
		int candidateChannel,float threshold)
{
  //It is applied to the reduced sampling frequency

  //Check that it is really a primary (it might be a secondary)
  //Primary: the average amplitude/chan in the first 200 ns > average from 200 to 600
  float integral1 = 0;
  float integral2 = 0;
  int timeWindow = (int)(600./samplingFreq);
  int timeWindow2 = (int)(200./samplingFreq);
  int chansOverThre1 = 0;
  int chansOverThre2 = 0;
  //cout << candidateChannel*samplingFreq << endl;
  for (int i=candidateChannel;i < min(candidateChannel+timeWindow,nsamples);i++)
    {
      if (i < (candidateChannel+timeWindow2)) 
	{
	  integral1 += pulse[i];
	  if (pulse[i] > threshold) chansOverThre1++;
	}
      else 
	{
	  integral2 += pulse[i];
	  if (pulse[i] > threshold) chansOverThre2++;
	}
    }

  if (!chansOverThre1) return false; //no fast channels!

  float ratioFast = 0.;
  float ratioSlow = 0.;
  ratioFast=integral1/timeWindow2; //chansOverThre1;
  ratioSlow=integral2/(timeWindow-timeWindow2); //chansOverThre2;

  //cout << ratioFast << " " << ratioSlow << " " <<  threshold << endl;

  //it was previously 0.6
  if (ratioSlow > 0.7*ratioFast) 
    {
      return false;
    }

  //Verify that it is not a tail of something else
  float previousIntegral = 0.;
  int numberOfStepsBefore = 0;
  for (int i=max(candidateChannel-timeWindow,0);i<candidateChannel;i++)
    {
      previousIntegral += pulse[i]; 
      numberOfStepsBefore++;
      if (pulse[i] < -1.0*threshold) //something nasty happening just before: too large undershoot
	{
	  if (abs(pulse[i]) > 0.3*pulse[candidateChannel])
	    return false; 
	}
    }
  //cout << "Previous: " << previousIntegral << " " << (integral1+integral2) << endl;
  if (previousIntegral > 0.5*(integral1+integral2))
    return false;

  return true; 
}

bool IsASecondary(float pulse[],int nChannelsToLook,int ionset_secondary,float dtres,
		  int nsamples,int ionset_primary)
{
  //Must be used with the pulse at 50 ns/chan
  int goodChan = 0;
  int numberOfStepsAfter = 0;
  float postIntegral = 0.;
  for (int i=ionset_secondary+1;i<min(ionset_secondary+nChannelsToLook,nsamples);i++)
    {
      if (pulse[i] > dtres) 
	goodChan++;
      postIntegral += pulse[i];
      numberOfStepsAfter++;
    }

  //cout << "Sec1: " << goodChan << " " << numberOfStepsAfter << endl;

  //Initially it was 0.3
  if (goodChan < (0.30*numberOfStepsAfter)+1)
    return false;

  //Verify it is not the tail of a primary
  int prevChan = 0;
  int numberOfStepsBefore = 0;
  float previousIntegral = 0.;
  //Sum down to the onset of the primary
  for (int i=max(ionset_secondary-nChannelsToLook,ionset_primary);i<ionset_secondary;i++)
    {
      if (pulse[i] > dtres) prevChan++;
      previousIntegral += pulse[i]; 
      numberOfStepsBefore++;
    }
  if (!numberOfStepsBefore || !numberOfStepsAfter) return false;

  if (numberOfStepsBefore > 5 && numberOfStepsAfter > 5)
    {
      float densityBefore = previousIntegral/((float)(numberOfStepsBefore));
      float densityAfter =  postIntegral/((float)(numberOfStepsAfter));
      //cout << "Sec1bis" << densityBefore << " " << densityAfter << endl;
      if (densityAfter < 0.65*densityBefore) //is was 0.5
	return false;
      //the pulse area is much smaller after the pulse than before! Cannot be a secondary
    }
  //cout << "Sec2: " << prevChan << " " << numberOfStepsBefore << endl;

  if (goodChan > (0.5*numberOfStepsAfter))
    {
      //cout << "Here1: " << (prevChan*numberOfStepsAfter/numberOfStepsBefore) << endl;
      if (prevChan/numberOfStepsBefore == 1) //in this case the condition with +1 can never be verified
	{
	  if (goodChan < (prevChan*numberOfStepsAfter/numberOfStepsBefore))
	    return false;
	}
      else
	{  
	  if (goodChan < (prevChan*numberOfStepsAfter/numberOfStepsBefore)+1)
	    return false;
	}
    }
  else 
    {
      if (goodChan < (2*prevChan*numberOfStepsAfter/numberOfStepsBefore))
	return false;
    }

  return true;
}

vector<float> LookForOtherSecondaries(float pulse[],int nsamples,float samplingFrequency,
				      float dtres,int ionset_secondary,
				      float primaryTime)
{
  const int MY_MAX_DATA_SIZE = 200000; 
  //This calss is meant to look for secondaries after the first one
  vector<float> secondaryTimes;
  secondaryTimes.push_back(ionset_secondary*samplingFrequency);
  
   // Re-bin at 500 ns/sample
  float pulseRebinned[MY_MAX_DATA_SIZE];
  float reducedSamplingFrequency = 500.0;
  int nsamples_rebin = (int) (nsamples*samplingFrequency/reducedSamplingFrequency);

  for (int k=0;k<nsamples_rebin;k++)
  {
    pulseRebinned[k] = 0.;
    for (int i=k*(nsamples/nsamples_rebin);i<min((k+1)*(nsamples/nsamples_rebin),nsamples);i++)
    {
      pulseRebinned[k] += pulse[i];
    }
  }

  //Differentiate pulse according to Newton's method
  float pulseDifferentiated[MY_MAX_DATA_SIZE];
  for (int k=1;k<(nsamples_rebin-1);k++)
    {
      pulseDifferentiated[k] = (pulseRebinned[k+1]-pulseRebinned[k-1])/(2.0*reducedSamplingFrequency);
    }
  pulseDifferentiated[0]=0.;
  pulseDifferentiated[nsamples_rebin-1]=0.;
  Smooth(pulseDifferentiated,nsamples_rebin,3);

  //1) Find maximum of the first secondary (- 2 mus, 4 mus)
  int ionset_newbinning = (int) (ionset_secondary*nsamples_rebin/nsamples);
  float diffFirstMax=0; 
  int positionFirstMax = 0;
  
  for (int k=ionset_newbinning-(int)(2000.0/reducedSamplingFrequency);
       k<min(ionset_newbinning+(int)(4000.0/reducedSamplingFrequency),nsamples_rebin);k++)
    {
      if (pulseDifferentiated[k]>diffFirstMax) 
	{
	  diffFirstMax = pulseDifferentiated[k];
	  positionFirstMax = k;
	}
    }

  //2) find global maximum (exclude primary)
  float diffGlobalMax=0;
  int positionGlobalMax=0;
  
  int kstart = std::max(0,(int) (primaryTime/reducedSamplingFrequency)+1);

  for (int k=kstart;k<nsamples_rebin;k++)
    {
      if (pulseDifferentiated[k]>diffGlobalMax) 
	{
	  diffGlobalMax = pulseDifferentiated[k];
	  positionGlobalMax = k;
	}
    }

  
  float differentialThreshold = 0;
  if (diffFirstMax < 0.8*diffGlobalMax)
    differentialThreshold = 0.7*diffFirstMax;
  else
    if (diffGlobalMax < 1)
      differentialThreshold = 0.08*diffGlobalMax; //test
    else
      differentialThreshold = 0.05*diffGlobalMax;

  //cout << "First max: " << positionFirstMax*reducedSamplingFrequency << " " << diffFirstMax << endl;
  //cout << "Global max: " << positionGlobalMax*reducedSamplingFrequency << " " << diffGlobalMax << endl;
  //float differentialThreshold = 0.08*diffFirstMax;
  //float differentialThreshold = 0.2*diffFirstMax;

  vector<float>* leftEdge = new  vector<float>;
  vector<float>* rightEdge = new vector<float>;
  cout << "Differential threshold " << differentialThreshold << endl;

  //2) look for windows containing maxima
  for (int k=ionset_newbinning-(int)(1000.0/reducedSamplingFrequency);
       k<nsamples_rebin;k++)
    {
      //float theLeft = 0.0;
      //float theRight = 0.0;
      if (pulseDifferentiated[k-1]<differentialThreshold && 
	  pulseDifferentiated[k] >= differentialThreshold)
	{
	  leftEdge->push_back(k*reducedSamplingFrequency);
	}
      if (pulseDifferentiated[k-1] > differentialThreshold && 
	  pulseDifferentiated[k] < differentialThreshold)
	{
	  rightEdge->push_back(k*reducedSamplingFrequency);
	}
    }

  /*
  cout << leftEdge->size() << " " << rightEdge->size() << " " << secondaryTimes[0] << endl;
  cout << differentialThreshold << endl;
  for (size_t i=0;i<leftEdge->size();i++)
    cout << (*leftEdge)[i] << endl;
  cout << "******" <<endl;
  for (size_t i=0;i<rightEdge->size();i++)
    cout << (*rightEdge)[i] << endl;
  */
  
  
  /*
  ofstream ofile("out2.dat");
  for (int k=0;k<nsamples_rebin;k++)
  {
    ofile << (k+0.5)*reducedSamplingFrequency << " " << pulseRebinned[k] << " " << 
      pulseDifferentiated[k] << endl;
  }
  ofile.close();
  */
  

  if (leftEdge->size() < rightEdge->size())
    {
      //In this case, the first left-edge is missing
      if (leftEdge->size() == 0)
	rightEdge->erase(rightEdge->begin());
      else
	{
	  int chann =  (int) (abs((*leftEdge)[0]-secondaryTimes[0])/reducedSamplingFrequency);
	  if ((*leftEdge)[0] > secondaryTimes[0] || chann ==0)  
	    rightEdge->erase(rightEdge->begin());
	  if ((*rightEdge)[0] < secondaryTimes[0])
	    rightEdge->erase(rightEdge->begin());
	}
    }
  else if (leftEdge->size() > rightEdge->size())
    {
      if (rightEdge->size() > 0)
	{
	  //In this case, the last right-edge is missing
	  if ((*leftEdge)[leftEdge->size()-1] > (*rightEdge)[leftEdge->size()-1])
	    {
	      leftEdge->erase(leftEdge->begin()+leftEdge->size()-1);
	    }
	}
      else //there is only one left edge
	leftEdge->erase(leftEdge->begin());
      //or there is a left edge too much
      if ((*leftEdge)[0] == 0 && leftEdge->size()) 
	leftEdge->erase(leftEdge->begin());
    }
  else if (leftEdge->size() == rightEdge->size()) //Dimensions are equal, but both previous cases are present
    {
      if (leftEdge->size() > 0)
	{
	  //for (size_t i=0;i<leftEdge->size();i++)
	  // cout << (*leftEdge)[i] << " --> " << (*rightEdge)[i] << " " <<  secondaryTimes[0] << endl;
	  float lastLeft = (*leftEdge)[leftEdge->size()-1];
	  float lastRight = (*rightEdge)[rightEdge->size()-1];
	  float firstLeft = (*leftEdge)[0];
	  float firstRight = (*rightEdge)[0];
	  if ((*leftEdge)[0] > secondaryTimes[0])  
	    {
	      if ((*leftEdge)[0] - secondaryTimes[0] < reducedSamplingFrequency)
		(*leftEdge)[0] = secondaryTimes[0]-reducedSamplingFrequency;
	      /*
	      else
		{
		  leftEdge->erase(leftEdge->begin());
		  rightEdge->erase(rightEdge->begin());
		}
	      */
	    }
	  if (lastLeft > lastRight && firstLeft > firstRight)
	    {
	      //for (size_t i=0;i<leftEdge->size();i++)
	      //	 cout << (*leftEdge)[i] << " --> " << (*rightEdge)[i] << " " <<  secondaryTimes[0] << endl;
	      leftEdge->erase(leftEdge->begin()+leftEdge->size()-1);
	      rightEdge->erase(rightEdge->begin());
	    }
	}
    }

  //cout << "Here!" << endl;

  if (leftEdge->size() != rightEdge->size()) 
    {
      cout << "Possibile problema!" << endl;
      cout << "*******************" << endl;
      cout << "Stop looking for secondaries" << endl;
      cout << "*******************" << endl;
      delete leftEdge;
      delete rightEdge;
      return secondaryTimes;
      //      exit(1);
    }

  //Ok, now remove from the list 
  // (1) the window containing the first secondary
  // (2) the windows narrower than 3 channels
  for (size_t h=0;h<leftEdge->size();h++)
    {
      if ((*leftEdge)[h] < secondaryTimes[0] && (*rightEdge)[h] > secondaryTimes[0])
	{
	  leftEdge->erase((leftEdge->begin()+h));
	  rightEdge->erase((rightEdge->begin()+h));
	  //cout << "Window containing first secondary" << endl;
	}
    }
  for (size_t h=0;h<leftEdge->size();h++)
    {
      if ((((*rightEdge)[h] - (*leftEdge)[h])/reducedSamplingFrequency) <= 3)
	{
       	  leftEdge->erase((leftEdge->begin()+h));
	  rightEdge->erase((rightEdge->begin()+h));
	  //cout << "Discarded because too narrow" << endl;
	}
    }

  /*
  cout << "Look for additional secondaries in "  << leftEdge->size() << " windows " << endl;
    for (size_t i=0;i<leftEdge->size();i++)
   cout << (*leftEdge)[i] << "-->" << (*rightEdge)[i] << endl;
  */

  //Loop on all the windows, get the maximum, and check if it is a secondary
  for (size_t h=0;h<leftEdge->size();h++)
    {
      int ledge = (int) ((*leftEdge)[h]/reducedSamplingFrequency);
      int redge = (int) ((*rightEdge)[h]/reducedSamplingFrequency);
      
      bool newsecondaryfound=false;
      int ionset_secondaryNew=0;
      int nChannelSec = (int) (1800./samplingFrequency);
      //int ionset_secondary0 = (int) 
      //	((positionFurtherMax-1)*(reducedSamplingFrequency/samplingFrequency));
      int ionset_secondary_start = (int) 
      	(ledge*(reducedSamplingFrequency/samplingFrequency));
      int ionset_secondary_stop = (int)
	(redge*(reducedSamplingFrequency/samplingFrequency));
      //Search for the first point over threshold (onset of the secondary)
      int istartChannel = max(ionset_secondary_start-50,1);
      //Check that it doesn't go in the previous secondary
      if (fabs(istartChannel*samplingFrequency-secondaryTimes[0])<2000.)
	{
	  istartChannel = max(ionset_secondary_start,1);
	}
      for (int i=istartChannel;i<min(ionset_secondary_stop+50,nsamples)
	     && !newsecondaryfound;i++)
	{
	  if (IsASecondary(pulse,nChannelSec,i,dtres,nsamples))
	    {
	      //cout << "Trovato:" << " " << (ionset_secondaryNew+0.5)*samplingFrequency << endl;
	      ionset_secondaryNew = i;
	      newsecondaryfound = true;
	    }
	}
	
      if (newsecondaryfound)
	{
	  secondaryTimes.push_back((ionset_secondaryNew+0.5)*samplingFrequency);
	}
    }

  delete leftEdge;
  delete rightEdge;

  /*
  ofstream ofile("out2.dat");
  for (int k=0;k<nsamples_rebin;k++)
  {
    ofile << (k+0.5)*reducedSamplingFrequency << " " << pulseRebinned[k] << " " << 
      pulseDifferentiated[k] << endl;
  }
  ofile.close();
  */

  return secondaryTimes;

}

int LookAgainForPrimary(float pulse[],int nsamples,float samplingfreq,
			float threshold,float firstSecTime)
{
  threshold = 1.2*threshold; //increase threshold to avoid single p.e.
  int isrcf = (int) (100./samplingfreq);
  int ionset_primary = 0;
  int lastChannel = nsamples-isrcf;
  if (firstSecTime)        
    {
      lastChannel = (int) (firstSecTime/samplingfreq) - isrcf;
    }
  for (int i=isrcf; i<max(isrcf,lastChannel) && ionset_primary==0;i++)
    {
      if (pulse[i] > threshold) 
	{
	  //Looks if this is a single phe
	  if(!IsASinglePhotoElectron(pulse,nsamples,samplingfreq,i,threshold))
	    ionset_primary=i;
	}
    }
  return ionset_primary;
}

vector<float> RefinePrimaryCalculations(float pulse[], int nsamples,float samplingfreq,
			       float primaryTimeLeft,float primaryTimeRight,
			       float firstSecTime,float threshold)
{
  /*Here I am supposed to: 
  1) define more precisely the position of primary, given the first guess
  2) calculate amplitude and testp's
  */
 
  //1) search for the first point over threshold (onset of the primary)
  int ionset_primary = 0;
  bool noiseflag = false;

  int ichanL = (int) (primaryTimeLeft/samplingfreq);
  int ichanR = (int) (primaryTimeRight/samplingfreq);
  for (int i=ichanL; i<ichanR && ionset_primary==0;i++)
    {
      if (pulse[i] > threshold) 
	  ionset_primary=i;
    }
  //If primary onset not found, iterate
  if (ionset_primary == 0)
    {
      for (int i=ichanL; i<ichanR && ionset_primary==0;i++)
	{
	  if (pulse[i] > threshold/2.) 
	    ionset_primary=i;
	}
    }

 
  //Integrate the primary signal for 7 mus, if the secondary is not there
  float amplitudeFull = 0.0;
  float amplitudeFast = 0.0;
  int timeWindow = (int) (7000./samplingfreq);
  int secChannel = (int) (firstSecTime/samplingfreq);
  if (!secChannel) secChannel = nsamples;
  //integrate up to 0.5 mus from the secondary
  int timeWindowBeforeSecondary = (int) (500./samplingfreq);
  
  int iChannelsFast = 0;
  int iChannelsTot = 0;
  int positiveChannels = 0;
  int negativeChannels = 0;
  //cout << "Window for primary: " << ionset_primary << " " <<  
  // min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow) << endl;
  float theMaxAmplitude = 0.;
  for (int i=ionset_primary;i<min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow);i++)
    {
      amplitudeFull += pulse[i]*samplingfreq; 
      if (pulse[i] > threshold)
	positiveChannels++;
      if (pulse[i] < -1.0*threshold)
	negativeChannels++;
      if (pulse[i] < -1.25*threshold)
	{
	  //find maximum in the 25 chans after ionset_primary
	  float primaryMaxCloseToOnset = pulse[ionset_primary];
	  for (int kkk=ionset_primary+1;kkk<min(ionset_primary+25,secChannel-timeWindowBeforeSecondary);
	       kkk++)
	    {
	      if (pulse[kkk] > primaryMaxCloseToOnset)
		primaryMaxCloseToOnset = pulse[kkk];
	    }
	  //cout << pulse[i] << " " << primaryMaxCloseToOnset << " " << ionset_primary << " " << i << endl;
	  float maxUndershoot = 0.1;
	  if (abs(i-ionset_primary)*samplingfreq > 200.) maxUndershoot = 0.25;
	  if (abs(pulse[i]) > maxUndershoot*primaryMaxCloseToOnset) //this is a noise! Large undershoot!
	      noiseflag = true;
	}
      iChannelsTot++;
      if (amplitudeFull > theMaxAmplitude) theMaxAmplitude = amplitudeFull;
      if ((i-ionset_primary)*samplingfreq < 80.0) 
	{
	  amplitudeFast += pulse[i]*samplingfreq; 
	  iChannelsFast++;
	}
    }
  //cout << "Pos & neg: " << positiveChannels << " " << negativeChannels << " " << threshold << " " << 
  // noiseflag << endl;
  if (negativeChannels > 0.4*positiveChannels)
	noiseflag = true;

  if (ionset_primary && !noiseflag)
    cout << "Corrected primary position: " << (ionset_primary+0.5)*samplingfreq << " ns" << endl;
  
  vector<float> results;
  
  if (noiseflag) //returns an empty vector
    {
      //cout << "Primary is likely to be noise" << endl;
      return results;
    }

  /*
  //In most cases, the baseline is not properly subtracted and the integral increases when no
  //signal is present. The idea is to calculate the integrale of the "baseline" for the final 
  //10% of the channels in the window, and then apply the correction.
  //For the full 7000 chan integration, it is 700 chans
  int timeWindowForBaselineCorrection = (int) (0.1*iChannelsTot);
  int channesUsedForBaselineCorrection = 0;
  float baselineCorrection = 0.;
  for (int i=ionset_primary+iChannelsTot;
       i>ionset_primary+iChannelsTot-timeWindowForBaselineCorrection;
       i--)
    {
      baselineCorrection += pulse[i]*samplingfreq; 
      channesUsedForBaselineCorrection++;
    }
  cout << "Baseline correction from: " << 
    ionset_primary+iChannelsTot-timeWindowForBaselineCorrection << " to " << 
    ionset_primary+iChannelsTot << endl;
  cout << channesUsedForBaselineCorrection << " channels" << endl;
  cout << baselineCorrection << " chan*ns" << endl;

  //Now correct amplitude full and amplitude fast
  cout << "Before correction: " << amplitudeFull << " ns*chan" << endl;
  if (channesUsedForBaselineCorrection)
    {
      amplitudeFast = amplitudeFast - 
	(baselineCorrection)*((float) iChannelsFast)/((float) channesUsedForBaselineCorrection);
      amplitudeFull = amplitudeFull - 
	(baselineCorrection)*((float) iChannelsTot)/((float) channesUsedForBaselineCorrection);
    }
  cout << "After correction: " << amplitudeFull << " ns*chan" << endl;
  */

  //Now I have to determine the rise-time 0-90%


  //float risetimethreshold = 0.9*amplitudeFull;
  float risetimethreshold = 0.9*theMaxAmplitude;
  float runningrisetime = 0.0;
  float theRiseTime = 0;
  int iRiseTime = ionset_primary;
  for (int i=ionset_primary;i<min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow) && 
	 runningrisetime<risetimethreshold ; i++)
    {
      runningrisetime += pulse[i]*samplingfreq; 
      iRiseTime = i;
      //  cout << i << " " << runningrisetime << " " << risetimethreshold << " " << amplitudeFull << " " << 
      //	endl;
    }
  theRiseTime = (iRiseTime-ionset_primary)*samplingfreq;

  //cout << "Primary amplitude total: "  <<  amplitudeFull << " (" << iChannelsTot << " channels)" << endl;
  //cout << "Primary amplitude fast (80 ns): "  <<  amplitudeFast << " (" << iChannelsFast << 
  // " channels), rise time: " << theRiseTime << endl;
   
  //if (amplitudeFull)
  // {
  //    float FastToTotal = amplitudeFast/amplitudeFull;
  // cout << "Fast to total: " << FastToTotal << endl;
  //  }

  results.push_back(amplitudeFull);
  results.push_back(amplitudeFast);
  float theTime = (float) ((ionset_primary+0.5)*samplingfreq);
  results.push_back(theTime);
  results.push_back(theRiseTime);
  results.push_back((float) iChannelsTot);
  results.push_back((float) iChannelsFast);
  return results;
}

vector<float> GetAmplitudesOfSecondaries(float pulse[],int nsamples,float samplingfreq,
					  vector<float> secPositions,float threshold,
					 int ionset_primary)
{
  vector<float> theResult;
  int timeWindow = (int) (30000./samplingfreq); //calculate secondary amplitude over 30 mus
  float amplitude = 0.0;
  int iChannelsTot = 0;
  
  /*
  cout << "Here: " << secPositions.size() << endl;
  for (size_t h=0;h<secPositions.size();h++)
    cout << secPositions[h] << endl;
  */

  //Loop over the secondaries
  for (size_t i=0;i<secPositions.size();i++)
    {
      bool noiseflag = false;
      int positiveChannels = 0;
      int negativeChannels = 0;
      amplitude = 0.0;
      iChannelsTot = 0;
      int startingChan = (int) (secPositions[i]/samplingfreq);
      int finalChan = startingChan+timeWindow;
      if (i != secPositions.size()-1)
	{
	  int nextSecondary = (int) (secPositions[i+1]/samplingfreq);
	  finalChan = min(startingChan+timeWindow,nextSecondary);
	}
      for (int j=startingChan;j<min(finalChan,nsamples);j++)
	{
	  amplitude += pulse[j]*samplingfreq; 
	  iChannelsTot++;
	  if (pulse[j] > threshold)
	    positiveChannels++;
	  else if (pulse[j] < -1.0*threshold)
	    negativeChannels++;
	}
      //cout << "Pos and neg: " << positiveChannels << " " << negativeChannels << " " << 
      // 	amplitude << " " << amplitude << endl;
      if (negativeChannels > 0.2*positiveChannels)
	noiseflag = true;

      //Flag negative amplitudes and strange events
      if (amplitude < 0 || noiseflag)
	amplitude = 0.0;
	
       theResult.push_back(amplitude);
    }

  float amplSum = 0;
  for (size_t i=0;i<theResult.size();i++)
    {
      amplSum += theResult[i];
    }
 
  //Remove fake secondaries having less than 1% of the total S2
  //(6% if we are close to the primary)
  float theThreshold = 0.01*amplSum;
  float primaryTime = ionset_primary*samplingfreq;
  for (size_t i=0;i<theResult.size();i++)
    {
      if (primaryTime > 10. && (secPositions[i]-primaryTime)<5000.)
	theThreshold = 0.06*amplSum;
      else
	theThreshold = 0.01*amplSum;
      if (theResult[i] < theThreshold)
	theResult[i] = 0.0;
    }

  return theResult;
}

//Provides the x and y position for each PMT filling 
//the two global variables x_pmt and y_pmt
int pmt_array_coord()
{
  x_pmt[ 0] =  -19.4856; y_pmt[ 0] =   11.2500;
  x_pmt[ 1] =  -13.8564; y_pmt[ 1] =   16.0000;
  x_pmt[ 2] =   -6.9282; y_pmt[ 2] =   20.0000;
  x_pmt[ 3] =    0.0000; y_pmt[ 3] =   22.5000;

  x_pmt[ 4] =  -20.7846; y_pmt[ 4] =    4.0000;
  x_pmt[ 5] =  -13.8564; y_pmt[ 5] =    8.0000;
  x_pmt[ 6] =   -6.9282; y_pmt[ 6] =   12.0000;
  x_pmt[ 7] =    0.0000; y_pmt[ 7] =   16.0000;
  x_pmt[ 8] =    6.9282; y_pmt[ 8] =   20.0000;

  x_pmt[ 9] =  -20.7846; y_pmt[ 9] =   -4.0000;
  x_pmt[10] =  -13.8564; y_pmt[10] =    0.0000;
  x_pmt[11] =   -6.9282; y_pmt[11] =    4.0000;
  x_pmt[12] =    0.0000; y_pmt[12] =    8.0000;
  x_pmt[13] =    6.9282; y_pmt[13] =   12.0000;
  x_pmt[14] =   13.8564; y_pmt[14] =   16.0000;

  x_pmt[15] =  -19.4856; y_pmt[15] =  -11.2500;
  x_pmt[16] =  -13.8564; y_pmt[16] =   -8.0000;
  x_pmt[17] =   -6.9282; y_pmt[17] =   -4.0000;
  x_pmt[18] =    0.0000; y_pmt[18] =    0.0000;
  x_pmt[19] =    6.9282; y_pmt[19] =    4.0000;
  x_pmt[20] =   13.8564; y_pmt[20] =    8.0000;
  x_pmt[21] =   19.4856; y_pmt[21] =   11.5000;
  
  x_pmt[22] =  -13.8564; y_pmt[22] =  -16.0000;
  x_pmt[23] =   -6.9282; y_pmt[23] =  -12.0000;
  x_pmt[24] =    0.0000; y_pmt[24] =   -8.0000;
  x_pmt[25] =    6.9282; y_pmt[25] =   -4.0000;
  x_pmt[26] =   13.8564; y_pmt[26] =    0.0000;
  x_pmt[27] =   20.7846; y_pmt[27] =    4.0000;

  x_pmt[28] =   -6.9282; y_pmt[28] =  -20.0000;
  x_pmt[29] =    0.0000; y_pmt[29] =  -16.0000;
  x_pmt[30] =    6.9282; y_pmt[30] =  -12.0000;
  x_pmt[31] =   13.8564; y_pmt[31] =   -8.0000;
  x_pmt[32] =   20.7846; y_pmt[32] =   -4.0000;

  x_pmt[33] =    0.0000; y_pmt[33] =  -22.5000;
  x_pmt[34] =    6.9282; y_pmt[34] =  -20.0000;
  x_pmt[35] =   13.8564; y_pmt[35] =  -16.0000;
  x_pmt[36] =   19.4856; y_pmt[36] =  -11.2500;
  
  return 37;
}

//Provides if a channel corresponds to a 3 inches
bool is_three_inches(int index) {

  if( (index ==  0) ||
      (index ==  3) ||
      (index == 15) || 
      (index == 21) ||  
      (index == 33) ||  
      (index == 36) ) return false;

  return true;

}
