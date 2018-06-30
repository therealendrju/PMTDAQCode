#include <vector>
#define MY_MAX_DATA_SIZE 400000
#define MY_MAX_PEAKS 1000
#define MY_MAX_NPMT 38
#define ML_FOR_PHOTONS 1

#include "wep.h"
#include "TH1F.h"

typedef struct {
	int nrun;
	int nevent;
        int evtime;
	int nn;
	float s1;
	float s2;
	int t1;
	int t2;
        int t2last;
        int t2largest;
	int ov;
        int issaturated[MY_MAX_NPMT];
        int isnoise[MY_MAX_NPMT];
        int satur_chan;
        float s1intetime;
        int nsec;
	float tm;
	float sf;
	int tp[MY_MAX_NPMT];
	float sp[MY_MAX_NPMT];
        float spf[MY_MAX_NPMT];
	float ss[MY_MAX_NPMT];
        float noise[MY_MAX_NPMT];
        float ppnoise[MY_MAX_NPMT];
        float baseline[MY_MAX_NPMT];
#ifdef ML_FOR_PHOTONS 
        float averagephetime;
        int numberofpheinwindow;
        float s1inwindow;
#endif
        float barx;
	float bary;
	float barxs;
	float barys;
        float maxS1ratio;
        int PMTofmaxS1ratio;
        float rtp;
        float rts;
	float fit[38];
	//temporary to check card jitter
	int cardtimes[(int)(MY_MAX_NPMT/2+0.5) ];
} my_event;


typedef struct 
{		
	int tstart;
	int tstop;
	double integ;
	double amp;
	int length;
	int npmt;
}peak;		
	
#ifdef _MY_PROC_CODE_

float adcc[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
double fftr[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
double ffti[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
bool isAboveThreshold[MY_MAX_DATA_SIZE*MY_MAX_NPMT];  
float fftspec[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float fftspec_filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float ave_freq_spect[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float adcc_filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float adccsum[MY_MAX_DATA_SIZE];
//added by A. for display purposes.
float adccsumcorr[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float adccraw[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float rmsOfPmts[MY_MAX_NPMT];
float pmtthreshold[MY_MAX_NPMT];
float baseline[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
float threshold;
float reducedSamplingFrequency;
float adccsumdown[MY_MAX_DATA_SIZE];
vector<float> phe_peak;
vector<peak> ser_peaks[MY_MAX_NPMT];


#else

extern float adcc[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern double fftr[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern double ffti[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float fftspec[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float fftspec_filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float ave_freq_spect[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float adcc_filt[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float baseline[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float adccsum[MY_MAX_DATA_SIZE];
//added by A. for display purposes.
extern float adccsumcorr[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float adccraw[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float rmsOfPmts[MY_MAX_NPMT];
extern float pmtthreshold[MY_MAX_NPMT];
extern bool isAboveThreshold[MY_MAX_DATA_SIZE*MY_MAX_NPMT];
extern float threshold;
extern vector<float> phe_peak;
extern vector<peak> ser_peaks[MY_MAX_NPMT];
extern float reducedSamplingFrequency;
extern float adccsumdown[MY_MAX_DATA_SIZE]; 

extern int init(char*,wep*);
extern int term(wep*);

extern int start_ev(void);
extern int end_ev(void);

extern int export_ev(wep*);
extern int make_fft(wep*);
extern int make_backfft(wep*);
extern int recog_peak(wep*);
extern int get_baseline(wep*);
extern int eval_s1_nofield(wep*);
extern int integrate(wep*);
extern int fill_values(wep*);
extern int fit_s1(wep*);

extern int process_pulse(wep*);
extern int sum_pmt(wep*);
extern int get_singlephe(wep*);
extern int dump_files(wep*);

extern TH1F *ser_hist[100];
extern TH1F *ser_leng[100];
extern TH1F *ser_amp[100];
//extern TH1F *ser_noise[100];
#endif

extern long int idx[];

