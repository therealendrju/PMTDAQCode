#ifndef __MY_TOOLS__
#define __MY_TOOLS__


#include <vector>

#define MY_MAX_NPMT 38

using namespace std;

extern float x_pmt[MY_MAX_NPMT];
extern float y_pmt[MY_MAX_NPMT];

float get_threshold(float pulse[],int nsamples,float samplingfreq,float pretrigger);
bool IsASinglePhotoElectron(float pulse[],int nsamples,float samplingFreq,
		int candidateChannel,float threshold);
bool IsAPrimary(float pulse[],int nsamples,float samplingFreq,
		int candidateChannel,float threshold);
bool IsASecondary(float pulse[],int nChannelsToLook,int ionset_secondary,float dtres,
			 int nsamples,int ionset_primary=0);
vector<float> LookForOtherSecondaries(float pulseRebinned[],int nsamples_rebin,
					     float reducedSamplingFrequency,
					     float dtres,int ionset_secondary,
					     float primaryTime);
int LookAgainForPrimary(float pulse[],int nsamples,float samplingfreq,
			float threshold,float firstSecTime);
vector<float> RefinePrimaryCalculations(float pulse[], int nsamples,float samplingfreq,
			       float primaryWindow1,float primaryWindow2,
			       float firstSecTime,float threshold);
vector<float> GetAmplitudesOfSecondaries(float pulse[],int nsamples,float samplingfreq,
						vector<float> secPositions,float threshold,
						int ionset_primary=0);
int pmt_array_coord();
bool is_three_inches(int ipmt);


#endif
