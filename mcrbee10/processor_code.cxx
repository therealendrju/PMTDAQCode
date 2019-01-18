#include <iostream>
#include <fstream>
#include <iomanip>
// #include <fftw3.h>
#include <vector>
#include <algorithm>
#include "math.h"

#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include "wep.h"

#define _MY_PROC_CODE_
#include "processor_code.h"

#include "tools.h"

#include "TH1F.h"
#include "TH2F.h"
#include <cmath>
///temporary
#include "../DataIO/libDataIO.h"
#include "TVector.h"
#include "TParameter.h"
#include "TObjString.h"

inline double myabs(double a) {return (a<0.0) ? -a : a;}

//extern void make_fit(double*, double, double, int, float*);

bool saturated;
TTree *tree;
TFile *ff;

TTree *ser_tree;

TH1F *ser_hist[100]={NULL};
TH1F *ser_leng[100]={NULL};
TH1F *ser_amp[100]={NULL};
//TH1F *ser_noise[100]={NULL};

my_event the_event;
peak my_peak;

//float pretrigger = 1900.;

int init(char* name,wep *ppp) {
	
	
	if(ppp->isSer)
	{
		for(int s=0;s<MY_MAX_NPMT;s++)
		{
			//cout<< "initializing histos" << endl;
			char *histname = new char[10];
			sprintf(histname, "ser_%d",s);
    //sprintf(histname2, "max_%d",s);
			ser_hist[s]=new TH1F(histname,"",1000,0.,300.);
			
			sprintf(histname, "length_%d",s);
    //sprintf(histname2, "max_%d",s);
			ser_leng[s]=new TH1F(histname,"",301,0.,300.);
			
			sprintf(histname, "amp_%d",s);
    //sprintf(histname2, "max_%d",s);
			ser_amp[s]=new TH1F(histname,"",1000,0.,300.);
			
			//sprintf(histname, "noise_%d",s);
    //sprintf(histname2, "max_%d",s);
			//ser_noise[s]=new TH1F(histname,"",1000,0.,300.);
			//cout << "initialising histo nr" << s <<" " <<ppp->nPMT << " " << MY_MAX_NPMT << endl;	
			
			ppp->isPMTOn.push_back(true);
			if(s>=ppp->nPMT)
				ppp->isPMTOn[s]=false;
  		 
		}
	
		ser_tree = new TTree("ser_tree","ser peak analysis");
		ser_tree->Branch("npmt",&my_peak.npmt,"npmt/I");	
		ser_tree->Branch("tstart",&my_peak.tstart,"tstart/I");
		ser_tree->Branch("tstop",&my_peak.tstop,"tstop/I");
		ser_tree->Branch("length",&my_peak.length,"length/I");
		ser_tree->Branch("amp",&my_peak.amp,"amp/D");
		ser_tree->Branch("integ",&my_peak.integ,"integ/D");
				
	
							
	}
	
	//std::exit(-1);
	cout << "init() called....." << endl;
	
	ff = new TFile(name,"recreate");
	tree = new TTree("tree","WArP analysis");
	
	tree->Branch("run",&the_event.nrun,"run/I");
	tree->Branch("event",&the_event.nevent,"event/I");
	tree->Branch("evtime",&the_event.evtime,"evtime/I");
	tree->Branch("nn",&the_event.nn,"nn/I");
	tree->Branch("s1",&the_event.s1,"s1/F");
	tree->Branch("s2",&the_event.s2,"s2/F");
	tree->Branch("t1",&the_event.t1,"t1/I");
	tree->Branch("t2",&the_event.t2,"t2/I");
	tree->Branch("t2last",&the_event.t2last,"t2last/I");
	tree->Branch("t2largest",&the_event.t2largest,"t2largest/I");
	tree->Branch("ov",&the_event.ov,"ov/I");
	//
	char appPMT[20];
	sprintf(appPMT,"issaturated[%i]/I",MY_MAX_NPMT);
	tree->Branch("issaturated",&the_event.issaturated,appPMT);
	//
	sprintf(appPMT,"isnoise[%i]/I",MY_MAX_NPMT);
	tree->Branch("isnoise",&the_event.isnoise,appPMT);
	//
	tree->Branch("satur_chan",&the_event.satur_chan,"satur_chan/I");
	tree->Branch("s1intetime",&the_event.s1intetime,"s1intetime/F");
	tree->Branch("nsec",&the_event.nsec,"nsec/I");
	tree->Branch("tm",&the_event.tm,"tm/F");
	tree->Branch("sf",&the_event.sf,"sf/F");

	sprintf(appPMT,"tp[%i]/I",MY_MAX_NPMT);
	tree->Branch("tp",&the_event.tp,appPMT);
	//
	sprintf(appPMT,"sp[%i]/F",MY_MAX_NPMT);
	tree->Branch("sp",&the_event.sp,appPMT);
	//
	sprintf(appPMT,"spf[%i]/F",MY_MAX_NPMT);
	tree->Branch("spf",&the_event.spf,appPMT);
	//
	sprintf(appPMT,"ss[%i]/F",MY_MAX_NPMT);
	tree->Branch("ss",&the_event.ss,appPMT);
	//
	sprintf(appPMT,"noise[%i]/F",MY_MAX_NPMT);
	tree->Branch("noise",&the_event.noise,appPMT);
	sprintf(appPMT,"ppnoise[%i]/F",MY_MAX_NPMT);
	tree->Branch("ppnoise",&the_event.ppnoise,appPMT);
	sprintf(appPMT,"baseline[%i]/F",MY_MAX_NPMT);
	tree->Branch("baseline",&the_event.baseline,appPMT);
	//temporary, to see the card time jitter
	sprintf(appPMT,"cardtimes[%i]/I",(int)(MY_MAX_NPMT/2+0.5));
	tree->Branch("cardtimes",&the_event.cardtimes,appPMT);
	
	//
	tree->Branch("rtp",&the_event.rtp,"rtp/F");
	tree->Branch("rts",&the_event.rts,"rts/F");
	tree->Branch("fit",&the_event.fit,"fit[6]/F");
	tree->Branch("barx",&the_event.barx,"barx/F");
	tree->Branch("bary",&the_event.bary,"bary/F");
	tree->Branch("maxS1ratio",&the_event.maxS1ratio,"maxS1ratio/F");
	tree->Branch("PMTofmaxS1ratio",&the_event.PMTofmaxS1ratio,
		     "PMTofmaxS1ratio/I");
	tree->Branch("barxs",&the_event.barxs,"barxs/F");
	tree->Branch("barys",&the_event.barys,"barys/F");
	//
#ifdef ML_FOR_PHOTONS 
	tree->Branch("averagephetime",&the_event.averagephetime,
		     "averagephetime/F");
	tree->Branch("numberofpheinwindow",&the_event.numberofpheinwindow,
		     "numberofpheinwindow/I");
	tree->Branch("s1inwindow",&the_event.s1inwindow,
		     "s1inwindow/F");
#endif

	//if (ppp->debug) cout << "init() done....." << endl;

	return 0;
}

int term(wep * ppp) {
	
	if(ppp->isSer)
	{
	for(int s=0;s<ppp->nPMT;s++){
		ser_hist[s]->Write();
		ser_amp[s]->Write();
		ser_leng[s]->Write();
		//ser_noise[s]->Write();	
    //phemax_hist[s]->Write();
		
		}
		ser_tree->Write();
		
		TParameter<float> threshold("threshold",ppp->threshold);
		TParameter<float> chan_before("chan_before",ppp->chan_before);
		TParameter<float> chan_after("chan_after",ppp->chan_after);
		TParameter<float> integr_window("integr_window",ppp->integr_window);
		
		cout << "++++ saving " << threshold.GetVal() << " " << chan_before.GetVal() << " " <<  chan_after.GetVal() << " " << integr_window.GetVal() << " " <<
		endl;
		threshold.Write();
		chan_before.Write();
		chan_after.Write();
		integr_window.Write();
		
	}
	else
	{
	tree->Write();

	//FILE *fp;
	//char hostnamet[30];
	//fp = popen("hostname -f","r");
	//fscanf(fp,"%s",hostnamet);
	char hostnamet[30];
	ifstream inp("../History.txt");

	int test_compare=1;

	if(inp.is_open())
		{
		while(!inp.eof())
			{
			inp.getline(hostnamet,30);
			test_compare=strncmp(hostnamet,"tag: warpanalysis",16);
			cout << "comparing" << endl;
			if(!test_compare)
				{
				cout << "writing analysis version " << hostnamet << endl; 
				break;
				}
			if(inp.fail())
				{
				cout << "breaking" << endl;
				sprintf(hostnamet,"tag error");
				break;
				
				
				}
			}
		}
	else
		{
		cout << "History file not found" << endl;
		sprintf(hostnamet,"no History file");
		}

	if(test_compare)
		{cout << "tag not found" << endl;
		sprintf(hostnamet,"no tag");
		}

	TObjString vtag(hostnamet);

	//TParameter< TString > version_tag("version_tag",vtag);
	//version_tag.Write();
	vtag.Write();

	inp.close();
	}
	ff->Close();
	
	return 0;
	
}

int start_ev(void) {
	
	saturated = false;
	
	the_event.nrun = 0; the_event.nevent = 0;
	for(int i=0; i<MY_MAX_NPMT; i++) 
	  {
	    the_event.tp[i]=0; 
	    the_event.sp[i]=0.0; 
	    the_event.ss[i]=0.0;
	    the_event.spf[i]=0.0;
	    the_event.noise[i] = 0.0;
	    the_event.ppnoise[i] = 0.0;
	    the_event.baseline[i] = 0.0;
	    the_event.issaturated[i] = 0;
	    the_event.isnoise[i] = 0;
	  }
	  for(int i=0; i<(MY_MAX_NPMT/2+0.5); i++) 
		  the_event.cardtimes[i]=0;
	the_event.s1 = 0.0; the_event.s2 = 0.0;
	the_event.t1 = 0; the_event.t2 = 0;
	the_event.t2last = 0; the_event.t2largest = 0;
	the_event.nn = 0; the_event.tm = 0.0; the_event.sf = 0.0;
	the_event.ov = 0; the_event.nsec=0;
	the_event.s1intetime=0.;
	the_event.rtp=0.;
	the_event.rts=0.;
	the_event.satur_chan = -1;
	the_event.barx = 0.;
	the_event.bary = 0.;
	the_event.barxs = 0.;
	the_event.barys = 0.;
	the_event.maxS1ratio=0.;
	the_event.PMTofmaxS1ratio = -1;

#ifdef ML_FOR_PHOTONS 
	the_event.averagephetime = 0.;
	the_event.numberofpheinwindow = 0;
	the_event.s1inwindow = 0.;
#endif

	return 0;
	
}


int fill_values(wep* p) {
	the_event.nrun = p->nRUN;
	the_event.nevent = p->nEVENT;
	the_event.evtime = p->event_time;
	return 0;
	
}  



inline int my_round(float a) {if(a>0) {return (int) (a+0.5);} else {return (int) (a-0.5);}}

/*
//Real baseline from MC
inline void eval_baseline_pmt(int nst, int* , float *pm_base) {
  float lbase=101.0;
  for(int it=0;it<nst; it++) pm_base[it]=lbase;
}
*/

// Funzione di FPP
/*
inline void eval_baseline_pmt(int nst, int* pm_evt, float *pm_base) {

  int iv, hi_max, ibase, isum, idt, idtm;
  float sumi, tbase;
  int hi_base[256];
  int pm_ave[MY_MAX_DATA_SIZE];
//
//  global baseline evaluation (most probable value)
//
      
  for(int ivalue=0; ivalue<256; ivalue++) hi_base[ivalue]=0;
  for(int it=0; it<nst; it++) hi_base[(pm_evt[it]+128)]++;

  hi_max=0; ibase=0;
  for(int ivalue=0; ivalue<256; ivalue++) {
    if(hi_base[ivalue]>hi_max) {
      hi_max = hi_base[ivalue];
      ibase=ivalue;
    }
  }
  ibase -= 128;

  tbase = (float) ibase;

//
// running sum over 10 bins 
//
  isum=10;
  sumi=1.0/((float) isum);
  for(int it=0; it<isum; it++) pm_ave[it]=0;
  for(int it=0; it<isum; it++) {
    pm_ave[isum-1]=pm_ave[isum-1]-pm_evt[it]+ibase;
  }
  for(int it=isum; it<nst; it++) {
    pm_ave[it]=pm_ave[it-1]-pm_evt[it]+pm_evt[it-isum];
  }

//  for(int i=0; i<500; i++) cout << i << "  " << pm_ave[i] << endl;

//
// baseline running sum running over "idt" bins
//

  idt=256;
  idtm=idt/2;

  for(int ivalue=0; ivalue<256; ivalue++) hi_base[ivalue]=0;
  for(int it=0; it<idt; it++) {
    iv=pm_ave[it]+128;
    if( (iv>=0) && (iv<256) ) hi_base[iv]++;
  }
  hi_max=0;
  for(int ivalue=0; ivalue<256; ivalue++) {
    if(hi_base[ivalue]>hi_max) {
      hi_max=hi_base[ivalue];
      ibase=ivalue-128;
    }
  }

  for(int it=0; it<idtm; it++) {
    pm_base[it]=tbase-ibase*sumi;s not active (but the 8th!)
  for (int ipmt=0;ipmt<nPMT;ipmt+
  }
  for(int it=idtm; it<(nst-idtm); it++) {
    iv=pm_ave[it-idtm]+128;
    if( (iv>=0) && (iv<256) ) {
      hi_base[iv]--;
    }
    iv=pm_ave[it+idtm]+128;
    if( (iv>=0) && (iv<256) ) {
      hi_base[iv]++;
    }
    hi_max=0;
    for(int ivalue=ibase+128-8; ivalue<ibase+128+9; ivalue++) {
      if([ivalue]>hi_max) {
	hi_max=hi_base[ivalue];
	ibase=ivalue-128;
      }
    }
    pm_base[it]=tbase-ibase*sumi;
  }
  for(int it=(nst-idtm); it<nst; it++) {
    pm_base[it]=tbase-ibase*sumi;
  }

}
*/

//Test per una routine di Luciano
/*
inline void eval_baseline_pmt(int nst, int* pm_evt, float *base) {

  int hi_max, ibase;
  float lbase;
  int hi_base[256];
  int it;
//
//  global baseline evaluation (most probable value)
//

  for(int ii=0; ii<256; ii++) hi_base[ii]=0;
  for(it=0; it<nst; it++) {
    if( (pm_evt[it]> 127) || (pm_evt[it]<-128) ) {
      cout << "WARNING: corrupted data !!" << endl;
    } else {
      hi_base[pm_evt[it]+128]++;
    }
  }
  hi_max=0; ibase=0;
  for(int iv=0; iv<256; iv++) {
    if(hi_base[iv]>hi_max) {
      hi_max=hi_base[iv];
      ibase=iv-128;
    }
  }
  //cout << "MPV: " << ibase << endl;
  //gets mean and sigma with respect to ibase between 1 and 3 mus
  int g_start, g_stop;
  float mean, sigma, sum_x, sum_x2, aa;

  sum_x=0.0; sum_x2=0.0;
  g_start = 1000; g_stop = 3000;
  for(it=g_start; it<g_stop; it++) {
    aa = pm_evt[it]-ibase;
    sum_x+=aa;
    sum_x2+=aa*aa;
  }

  mean=sum_x/((float) (g_stop-g_start));
  sigma=sqrt(sum_x2/((float)(g_stop-g_start)) - mean*mean);
  //cout << "mean: " << mean << endl;


  float lbase0 = ibase + mean;

  //Local baseline evaluation
  for(it=0; it<nst-256; it+=256) {
    //if (it)
    //  lbase = base[it-1];
    //else
      lbase = lbase0; //this is the standard. If there is no signal, take the calculated one
    int pointsAbove = 0;
    int pointsBelow = 0;
    float inte = 0.0;
    for(int iv=0; iv<256; iv++) 
      {
	inte += pm_evt[it+iv];
	if (pm_evt[it+iv] > lbase0) 
	  pointsAbove++;
	else
	  pointsBelow++;
      }
    if (pointsAbove && pointsBelow)
      {
	float relerr = sqrt((1./((float)pointsAbove))+(1./((float)pointsBelow)));
	float ratio = ((float)(pointsAbove))/((float)(pointsBelow));
	float abserr = relerr*ratio;
	//Check if the number above != number below
	if (fabs(ratio-1.) < 2.0*abserr)
	  {
	    //real baseline here! 
	    lbase = inte/256.;
	  }
	//else 
	// if (it) lbase = base[it-1];
	//cout << it << " " << ratio << " " << relerr << " " << inte/256. << " " << 
	// pointsAbove << " " << pointsBelow << endl;
      }
    for(int iv=0; iv<256; iv++) base[it+iv]=lbase;
  }
  
  for(;it<nst; it++) base[it]=lbase;
}
*/

// Routine di Alfredo (correzioni di Luciano) 

inline void eval_baseline_pmt(int nst, int* pm_evt, float *base,float pretrigger,bool is_CAEN) {

  int hi_max, ibase, nn;
  float sum, offset, lbase;
  
  int hi_base[1024];
    

if(!is_CAEN)
{
  //int hi_base[256];


// global baseline evaluation (most probable value)


  for(int ii=0; ii<256; ii++) hi_base[ii]=0;
  for(int it=0; it<nst; it++) {
    if( (pm_evt[it]> 127) || (pm_evt[it]<-128) ) {
      cout << "WARNING: corrupted data !!" << endl;
    } else {
      hi_base[pm_evt[it]+128]++;
    }
  }
  hi_max=0; ibase=0;

//the 5 is to avoid saturated events deforming the baseline value.
  for(int iv=5; iv<256; iv++) {
    if(hi_base[iv]>hi_max) {
      hi_max=hi_base[iv];
      ibase=iv-128;
    }
  }
}  
else
{

  for(int ii=0; ii<1023; ii++) hi_base[ii]=0;
  for(int it=0; it<nst; it++) {
    if( (pm_evt[it]> 1023) || (pm_evt[it]<0) ) {
      cout << "WARNING: corrupted data !! value:" << pm_evt[it]  << endl;
    } else {
      hi_base[pm_evt[it]]++;
    }
  }
  hi_max=0; ibase=0;

//the 5 is to avoid saturated events deforming the baseline value.
  for(int iv=5; iv<1023; iv++) {
    if(hi_base[iv]>hi_max) {
      hi_max=hi_base[iv];
      ibase=iv;
    }
  }


}



 // cout << "+++ ibase " << ibase << endl;
  //gets mean and sigma with respect to ibase between 1 and 3 mus
  int g_start, g_stop;
  float mean, sigma, sum_x, sum_x2, aa;

  sum_x=0.0; sum_x2=0.0;
  g_start = (int)(0.25*pretrigger); g_stop = (int)(0.75*pretrigger);
  for(int it=g_start; it<g_stop; it++) {
    aa = pm_evt[it]-ibase;
    sum_x+=aa;
    sum_x2+=aa*aa;
  }

  mean=sum_x/((float) (g_stop-g_start));
  sigma=sqrt(sum_x2/((float)(g_stop-g_start)) - mean*mean);
  float mean0 = mean;
  float sigma0 = sigma;

  //cout << "mean0 = " << mean << "\t sigma0 = " << sigma << endl; 
  sum_x=0.0; sum_x2=0.0; nn=0;
  g_start = 0; g_stop = nst;
  //goes over all channels. Gets mean and sigma for events within 3sigmas
  for(int it=g_start; it<g_stop; it++) {
    if( fabs(pm_evt[it]-(ibase+mean0))<3.0*sigma ) {
      aa = pm_evt[it]-ibase-mean0;
      sum_x+=aa;
      sum_x2+=aa*aa;
      nn++;
    }
  }

  mean=sum_x/((float) nn);
  sigma=sqrt(sum_x2/((float) nn) - mean*mean);

  //cout << "mean = " << mean << "\t sigma = " << sigma << endl; 

//
//  local baseline evaluation
//
// Warning: there is the possibility that nn = 0, namely for 0.5 mus all samples
// are >2 sigma away from the mean. This may cause a major problem. lbase was 
// initialized to 0. Now I set it to ibase. It is not very satisfactory, but the 
// mistake is a bit less heavy. If nn=0, the baseline is set equal to the one of 
// the previous sample.
  int it;
  

  
  offset = 0.0; lbase = 0;
  for(it=0; it<nst-256; it+=256) {
    lbase = ibase+mean0;  
    sum = 0.0; nn=0;
    for(int iv=0; iv<256; iv++) {
      //I take the difference with respect to (ibase+mean0) than ibase, because the shift mean0 
      //with respect to the mpv is known
      if( fabs(pm_evt[it+iv]-(ibase+mean0)) < max(2.0*sigma,2.0*sigma0) ) {nn++; sum+=pm_evt[it+iv];} 
    }

    if(nn>0)
	lbase = sum/((float) nn);
    else
      if (it)
	lbase = base[it-1]; //previous one

    for(int iv=0; iv<256; iv++) base[it+iv]=lbase;
  }


for(;it<nst; it++) base[it]=lbase;



}


// Alfredo v3
// mean value before S1 
/*
void eval_baseline_pmt(int ns, int* dat, float *base) {
 
  int hist[256];
  int histmax, imax;
 
  for(int i=0; i<256; i++) hist[i]=0;
 
  for(int is=500; is<4000; is++) hist[dat[is]+128]++;
 
  histmax = 0; imax = 0;
  for(int i=0; i<256; i++) {
    if(hist[i]>histmax) {
      imax=i-128;
      histmax=hist[i];
    }
  }
  
  float sum=0.0;
  for(int is=0; is<3500; is++) sum+=dat[is]-((float) imax);
  sum /= 3500.0;
  
  sum += (float) imax;
  
  for(int is=0; is<ns; is++) base[is]=sum;
}
*/


//Routine di Claudio Montanari (test)
/*
inline void eval_baseline_pmt(int nst, int* pm_evt, float *base) {
  //cout << "entro" << endl;
  //Compute baseline from 20 ns and for 2 mus
  int ifirst = 20; 
  int ilast = 4000; 
  if (ilast <= ifirst)
    {
      cout << "Pretrigger is too short" << endl;
      exit(1);
    }
  if (ilast>nst) 
    { exit(1);}
  double width = (double) (ilast-ifirst+1);
  double basel=0.0;
 
  for (int i=ifirst;i<ilast;i++)
    basel += (double) pm_evt[i];
  basel /= width;

  //cout << "Base media" << basel << endl;

  //Compute rms_of_baseline
  double rmsbasel=0.0;
  for (int i=ifirst;i<ilast;i++)
    rmsbasel += (pm_evt[i]-basel)*(pm_evt[i]-basel);
  rmsbasel /= (double) (ilast-ifirst);
  rmsbasel = sqrt(rmsbasel);
  float dtres = 2.0*rmsbasel;

  //cout << "Rms " << rmsbasel << endl;

  //Now compute a running baseline 
  int ist = ifirst + (ilast-ifirst)/2;
  int ils = nst-ist-1;
  //int iwidth = (int) width;
  for (int i=0;i<ist;i++)
    base[i]=basel;
  for (int i=ils;i<nst;i++)
    base[i]=basel;
  //
  //This is not a real loop
  //attenzione, sto mettendo base[i-1] al posto di basel!
  int ichans = 0;
  for (int i=ist;i<ist+1;i++)
    {
      base[i]=0.0;
      for (int ii=(i-(int) (width/2));ii<(i+(int) (width/2));ii++)
	{
	  //This condition avoids calculating baseline on peaks
	  if (pm_evt[ii]<(basel+dtres) && pm_evt[ii] > (basel-dtres))
	    {
	      base[i] += pm_evt[ii];
	      ichans++;
	    }	 
	}
      base[i] /= ichans;
    }
  //This is a real loop
   for (int i=ist+1;i<ils;i++)
     {
       base[i] = base[i-1]*ichans;
       //subtract the last channel on the left, add the first on the right
       int ileft = i-1-(int) (width/2);
       int iright = i-1+(int) (width/2);
       ichans--;
       if (pm_evt[ileft]<(basel+dtres) && pm_evt[ileft] > (basel-dtres))
	 {base[i] -= pm_evt[ileft];}
       else
	 ichans++;
       if (pm_evt[iright]<(basel+dtres) && pm_evt[iright] > (basel-dtres))
	 {
	   base[i] += pm_evt[iright];
	   ichans++;
	 }	 
       
       base[i] /= ichans;
       //cout << i << " " << base[i] << " " << ichans << endl;
     }
}
*/


int get_baseline(wep* p) 
{
  //cout << "Get baseline" << endl;
  int nsamples = p->nSAMPLES;
  int npmt = p->nPMT;
  
  int *pin = p->data;
  float *pout = adcc;

  float pretrig= (float)(p->preTRIG-100.);

  if(pretrig<0)
    pretrig=fabs(pretrig);
  

  for(int ipmt=0; ipmt<npmt; ipmt++) {
  //  cout << ipmt << " " << idx[ipmt] << endl;
    eval_baseline_pmt(nsamples, &(pin[idx[ipmt]]), &(baseline[idx[ipmt]]),pretrig,p->is_CAEN);
   }
    
  int firstSaturationChan = nsamples+1;
std::cout << "++++ number of PMTS " << npmt << " isPMTOn size " << p->isPMTOn.size() << std::endl; 
  for(int ipmt=0; ipmt<npmt; ipmt++) {
    //Consider only ACTIVE PMTs
    int sat_level = -127;  
      if(p->is_CAEN)
      {
	sat_level = 1; //for CAEN board
      }
	
      if(p->isPMTOn[ipmt]) 
	{
	for(int i=0; i<nsamples; i++) {
	if(pin[idx[ipmt]+i] <= sat_level) 
	{
	  saturated=true;
	  if (i<firstSaturationChan) firstSaturationChan = i;
	  if (the_event.issaturated[ipmt] == 0)
	  the_event.issaturated[ipmt] = 1;
	}
	pout[idx[ipmt]+i] = baseline[idx[ipmt]+i] - (float) pin[idx[ipmt]+i];
	adccraw[idx[ipmt]+i] =  (float) pin[idx[ipmt]+i];		  
	}

		
	}
 
   }
    
  if(saturated) 
    {
      the_event.ov = 1;
      the_event.satur_chan = firstSaturationChan;
      cout << "Saturated event" << endl;
    }
  return 0;
}

int integrate(wep *p) {
	
	int npmt, nsamples;
	float *pin;
	float *pout;
	
	double sum;
	
	nsamples = p->nSAMPLES;
	npmt = p->nPMT;
	
	pin = adcc;
	pout = filt;
	
	for(int ipmt=0; ipmt<npmt; ipmt++) {
		sum = 0.0;
		for(int i=0; i<nsamples; i++) {
			sum += pin[idx[ipmt]+i];
			pout[idx[ipmt]+i] = sum;
		}
	}
	
	return 0;
	
}

int ff_nn;     

int fit_s1(wep *p) {

  float *pin;

  int npmt, nsamples, iii;
  double fitarray[2000];

  int ff_rebin = 40;
  double ff_sample = 0.001*ff_rebin;
  int ff_size;   //  = 7000; // ns
 
  int s1_start, s1_end;
  float samplingfreq;
  pin = adcc;
  npmt = p->nPMT;
  nsamples = p->nSAMPLES;
  samplingfreq = p->get_sampling(); //ns/chan
  s1_start = the_event.t1;
  s1_end = s1_start + (int) (the_event.s1intetime/samplingfreq);

  cout << "S1 start = " << s1_start << " S1 end = " << s1_end << endl; 
  
  ff_size = s1_end - s1_start;
  if(s1_start < 4000) return 0;
  if((s1_start+ff_size) >= nsamples) return 0;
  ff_nn = ff_size / ff_rebin;
  if(ff_nn > 200) return 0;

  iii=0;
  for(int is=s1_start; is<=s1_end; is+=ff_rebin) {
    fitarray[iii] = 0.0;
    for(int ipmt=0; ipmt<npmt; ipmt++) {
      for(int ij=0; ij<ff_rebin; ij++) fitarray[iii] += (double) pin[idx[ipmt]+is+ij];
    }
    if(fitarray[iii] < 0.0) fitarray[iii] = 0.0;
    iii++;
    if(iii==2000) break;
  }
  ff_nn = iii;

  //make_fit(fitarray, ff_sample, the_event.s1, ff_nn, the_event.fit);
  return 0;

}

int eval_s1_nofield(wep *p) {
	
	int npmt, nsamples;
	float *pin;
	float sum,sum1,sum2;
	
	int s1_start, s1_end;
	//   s1_start=4940; s1_end=6940;  // signals r953
	s1_start=4940; s1_end=10940;  // signals r954
	//  s1_start=4940; s1_end=5000;     // SERs
	//   s1_start=940; s1_end=1000;     // SERs
	
	nsamples = p->nSAMPLES;
	npmt = p->nPMT;
	
	// Evaluate and subtract baseline data -> adcc
	
	pin = adcc;
	
	the_event.s1=0.0;
	for(int ipmt=0; ipmt<npmt; ipmt++) {
		sum = 0.0; sum1 = 0.0; sum2 = 0.0;
		for(int is=s1_start; is<s1_end; is++) {
			sum += pin[idx[ipmt]+is];
			if(is<5000) {
				sum1 += pin[idx[ipmt]+is];
			} else {
				sum2 += pin[idx[ipmt]+is];
			}
		}
		the_event.sp[ipmt] = sum1;
		the_event.ss[ipmt] = sum2;
		the_event.s1+=sum;
		
		//     if(ipmt==2) make_fit(&pin[idx[ipmt]]);
		
	}
	
	return 0;
	
}

int end_ev(void) {
	tree->Fill();
	
	
	//   fout.precision(2);
	//   fout.setf(ios::floatfield,ios::fixed);
	//   fout.setf(ios::adjustfield,ios::right);
	
	//   fout << setw(8) << the_event.nrun << setw(8) << the_event.nevent << setw(4) << the_event.nn 
	//        << setw(10) << the_event.s1 << setw(12) << the_event.s2 << setw(8) << the_event.t1 << setw(8) << the_event.t2
	//        << setw(4) << (saturated ? 1 : 0) << setw(10) << the_event.tm << setw(10) << the_event.sf
	//        << setw(8) << the_event.tp[0] << setw(8) << the_event.tp[1] << setw(8) << the_event.tp[2] << setw(8) << the_event.tp[3]
	//        << setw(10) << the_event.sp[0] << setw(10) << the_event.sp[1] << setw(10) << the_event.sp[2] << setw(10) << the_event.sp[3]
	//        << setw(10) << the_event.ss[0] << setw(10) << the_event.ss[1] << setw(10) << the_event.ss[2] << setw(10) << the_event.ss[3] << endl;
	
	return 0;
	
}

/////// LUCA

int export_ev(wep* p) {
	
	int npmt, nsamples, nevent, nrun;
	int *pin;
	char namefile[100];
	//	char namedir[100];
	FILE *export_file;
	
	nsamples = p->nSAMPLES;
	npmt = p->nPMT;
	nevent = p->nEVENT;
	nrun = p->nRUN;
	
	
	//sprintf(namefile,"Export/%d_%05d.txt",nrun,nevent);
	sprintf(namefile,"Export/%d_%05d.txt",nrun,nevent);
	export_file = fopen(namefile,"w");
	
	pin  = p->data;
	//pout = adcc;
	
	// Export to a file containing 5 columns: the sample number, PMT[0], PMT[1], PMT[2], PMT[3] 
	for(int i=0; i<nsamples; i++) {
		fprintf(export_file,"%d %d %d %d %d\n",i,pin[idx[0]+i],pin[idx[1]+i],pin[idx[2]+i],pin[idx[3]+i]);
    }
	
	fclose(export_file);
	return 0;
}

//FFT di Luca
/*
int make_fft(wep* p) {
	
	// (When you compile, you must also link with the fftw3 library, e.g. -lfftw3 -lm on Unix systems.)
	// Users should note that FFTW computes an unnormalized DFT. Thus, computing a forward followed 
	// by a backward transform (or vice versa) results in the original array scaled by nsamples.
	// f_n=n/(nsamples*delta) with n=-nsamples/2.....+nsamples/2 where delta is the sampling interval. 
	// This routine provides only positive frequency since out[n]=out[nsamples-n]
	int npmt, nsamples, nevent, nrun, N;
	float *pin;
	//int *pin;
	double *poutr, *pouti;//Real and Imaginary part of the fftw
	float *poutm,*pouts;//spectrum magnitude
	nsamples = p->nSAMPLES;
	npmt = p->nPMT;
	nevent = p->nEVENT;
	nrun = p->nRUN;
	N = nsamples/2+1;// Lenght of an fft of a real array nsamples long (without redundancy). 
	
	double in[nsamples];
	double magnitude;
	fftw_complex *out;
	fftw_plan plan;
	
	//pin  = p->data;
	pin = adcc;
	poutr = fftr;
	pouti = ffti;
	poutm = ave_freq_spect;
	pouts = fftspec;
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	
	for(int ipmt=0; ipmt<npmt; ipmt++) {  
		
		plan = fftw_plan_dft_r2c_1d(nsamples, in, out,FFTW_PRESERVE_INPUT);
		//Fill the input
		for(int i=0; i<nsamples; i++) {
			in[i] = (double) (pin[idx[ipmt]+i]);
			//if(i<10) printf("previous PMT=%d index=%d in=%lf %d\n",ipmt,idx[ipmt]+i,in[i],pin[idx[ipmt]+i]);
		}
		
		fftw_execute(plan);//execute the fft 
		for(int i=0; i<nsamples; i++) {
			//if(i<10) printf("after PMT=%d index=%d in=%lf %d\n",ipmt,idx[ipmt]+i,in[i],pin[idx[ipmt]+i]);
		}
		//Store the complex output real and imaginary part
		for(int i=0; i<N; i++) {
			poutr[(ipmt*N)+i] = out[i][0];//out[i][0] real
			pouti[(ipmt*N)+i] = out[i][1];//out[i][1] imaginary
			pouts[(ipmt*N)+i] = TMath::Sqrt(TMath::Power(poutr[(ipmt*N)+i],2)+TMath::Power(pouti[(ipmt*N)+i],2));
			magnitude=TMath::Sqrt(TMath::Power(poutr[(ipmt*N)+i],2)+TMath::Power(pouti[(ipmt*N)+i],2));
			poutm[(ipmt*N)+i] = poutm[(ipmt*N)+i]+magnitude;
			//if(i<10) printf("Event=%d PMT=%d i=%d in[i]=%lf poutr=%f pouti=%f\n",nevent,ipmt,i,in[i],poutr[(ipmt*N)+i],pouti[(ipmt*N)+i]);
		}
	}
	
	fftw_destroy_plan(plan);
	fftw_free(out);
	
	return 0;
}
*/
// int make_backfft(wep* p) {
	
// 	// (When you compile, you must also link with the fftw3 library, e.g. -lfftw3 -lm on Unix systems.)
// 	// Users should note that FFTW computes an unnormalized DFT. Thus, computing a forward followed 
// 	// by a backward transform (or vice versa) results in the original array scaled by nsamples.
// 	// While filling the complex input you can remove some frequencies
	
// 	int npmt, nsamples, nevent, nrun, N;
// 	double *pinr, *pini;//Real and Imaginary part of the fftw
// 	float *pout,*pouts;
// 	int filt=0;
	
// 	float freq_low=275000;// Hz
// 	float freq_high=312500;// Hz
// 	float freq_max = 1./(2*0.000000001);	
//     int cut_low, cut_high;
// 	float amp_low, amp_high, amp, amp_filt, attenuation;
// 	int bin_ave=1;
	
// 	nsamples = p->nSAMPLES;
// 	npmt = p->nPMT;
// 	nevent = p->nEVENT;
// 	nrun = p->nRUN;
// 	N = nsamples/2+1;// Lenght of an fft of a real array nsamples long (without redundancy). 
// 	pout = adcc_filt;
	
// 	double out[nsamples];
// 	fftw_complex *in;
// 	fftw_plan plan;
	
// 	pinr = fftr;
// 	pini = ffti;
// 	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
// 	pouts = fftspec_filt;
// 	cut_low = (int) ((freq_low)/(freq_max/(N-1)));
// 	cut_high = (int) ((freq_high)/(freq_max/(N-1)));
// 	printf("Low cut=%d High cut=%d\n",cut_low,cut_high);
	
// 	for(int ipmt=0; ipmt<npmt; ipmt++) {  
		
// 		plan = fftw_plan_dft_c2r_1d(nsamples, in, out,FFTW_PRESERVE_INPUT);//define the back fft, the output is n/2+1 long
		
// 		if(filt == 0){
// 			for(int i=0; i<N; i++) {
// 				in[i][0] = pinr[(ipmt*N)+i];
// 				in[i][1] = pini[(ipmt*N)+i];
// 			}
// 		}
// 		else if(filt == 1){
// 			amp_low = 0;
// 			amp_high = 0;
// 			for(int s=0;s<bin_ave;s++){
// 				//Averaging the mean value in the few bins before the low frequency and above the high frequency
// 				amp_low = amp_low + TMath::Sqrt(TMath::Power(pinr[(ipmt*N)+cut_low-s],2)+TMath::Power(pini[(ipmt*N)+cut_low-s],2));
// 				amp_high = amp_high + TMath::Sqrt(TMath::Power(pinr[(ipmt*N)+cut_high+s],2)+TMath::Power(pini[(ipmt*N)+cut_high+s],2));
// 			}
// 			amp_low = amp_low/bin_ave;
// 			amp_high = amp_high/bin_ave;
// 			for(int i=0; i<N; i++) {
// 			    if((i<cut_low)||(i>cut_high)){	
// 					in[i][0] = pinr[(ipmt*N)+i];
// 					in[i][1] = pini[(ipmt*N)+i];
// 					pouts[(ipmt*N)+i]=TMath::Sqrt(TMath::Power(pinr[(ipmt*N)+i],2)+TMath::Power(pini[(ipmt*N)+i],2));
// 				}
// 				if((i>=cut_low)&&(i<=cut_high)){
// 					amp_filt = amp_low * TMath::Exp((1./(cut_high-cut_low)) * TMath::Log(amp_high/amp_low) * (i-cut_low));
// 					amp = TMath::Sqrt(TMath::Power(pinr[(ipmt*N)+i],2)+TMath::Power(pini[(ipmt*N)+i],2));
// 					attenuation = amp/amp_filt;
// 					printf("PMT=%d Index=%d Attenuation=%f\n",ipmt,i,attenuation);
// 					in[i][0] = pinr[(ipmt*N)+i]/attenuation;
// 					in[i][0] = pini[(ipmt*N)+i]/attenuation;
// 					pouts[(ipmt*N)+i]=amp/attenuation;
// 				}
// 			}
// 		}
		
// 		fftw_execute(plan);//execute the backfft 
		
// 		for(int i=0; i<nsamples; i++) {
// 			pout[(ipmt*nsamples)+i] = (float) (out[i]/nsamples);
// 		}
// 	}
	
// 	fftw_destroy_plan(plan);
// 	fftw_free(in);
	
// 	return 0;
// }

// //int recog_peak(wep* p) {
// //	
// //	int npmt, nsamples, nevent, nrun;
// //	int window = 10; 
// //	int j_average;
// //    float average;
// //	float *pin;
// //	float *pout;
// //	
// //	//pin = p->data;
// //	pin = adcc;
// //	pout = filt;
// //	
// //	nsamples = p->nSAMPLES;
// //	npmt = p->nPMT;
// //	nevent = p->nEVENT;
// //	nrun = p->nRUN;
// //	
// //	float undersampled[nsamples];
// //	
// //	for(int ipmt=0; ipmt<npmt; ipmt++) {  
// //		
// //		for(int i=0; i<(nsamples); i++) {
// //		    
// //			undersampled[(int)(i/window)] = 0;
// //			for(int j=0;j<window;j++){
// //				undersampled[(int)(i/window)] = undersampled[(int)(i/window)] +  pin[idx[ipmt]+i];
// //			}
// //			
// //		}
// //	}
// //}
// //
// //return 0;
// //
// //}


// int recog_peak(wep* p) {
	
// 	int npmt, nsamples, nevent, nrun;
// 	int window = 10; 
// 	int j_average;
//     float average;
// 	float *pin;
// 	float *pout;
	
// 	//pin = p->data;
// 	pin = adcc_filt;
// 	pout = filt;
	
// 	nsamples = p->nSAMPLES;
// 	npmt = p->nPMT;
// 	nevent = p->nEVENT;
// 	nrun = p->nRUN;
	
// 	for(int ipmt=0; ipmt<npmt; ipmt++) {  
		
// 		for(int i=0; i<(nsamples); i++) {
// 			average=0;
// 			j_average=0;
// 			for(int j=0; j<window; j++){
// 				if((i-window/2+j>=0.)&&((i-window/2+j)<nsamples)){
// 					average = average+pin[idx[ipmt]+i-window/2+j];
// 					j_average++;
// 					//printf("PMT=%d index=%d j=%d j_ave=%d\n",ipmt,i,j,j_average);
// 				}
// 			}
// 			average = average/j_average;
// 			pout[idx[ipmt]+i] = average;
// 		}
		
// 	}
	
// 	return 0;
	
// }


// LUCIANO
/*Here there are the external functions that I need */

vector<float> GetAmplitudesOfSecondariesInThePMTs(wep* p,vector<float> secPositions,int maxSecondary);
void GetAmplitudesOfPrimariesInThePMTs(wep* p,
				       float primaryTimeLeft,float primaryTimeRight,
				       float primaryTime,
				       float firstSecTime);
void RefineBaselineAndSum(wep* p,float primTime, float firstSec, float lastSec,float pretrigger);
void CalculateIntegralsForPrimaries(wep* p,float primTime, float firstSec);

/*Routine to get the sum pulse */
int sum_pmt(wep *p) 
{	
  int nsamples;
  float *pin;
  float *pout;
  
  float sum;
  
  nsamples = p->nSAMPLES;
  float samplingfreq = p->get_sampling(); //ns/chan
	
  pin = adcc;
  pout = adccsum;
	
  for (int i=0;i<nsamples;i++)
    {
      sum = 0.0;
      for (int ipmt=0;ipmt<p->nPMT;ipmt++)
	{
	  if (p->isPMTOn[ipmt]) //sum only active PMTs
	    {
	      //cout << "Summing: " << ipmt << endl;
	      sum += (float) pin[i+ipmt*nsamples];
	    }
	}
      pout[i]=sum;
    }

//to sum up the average signal, a baseline is subtracted for each 16th channel to
// correct for board dephasing
if(p->isAverage)
    {
    
    
double *hhsin=new double[nsamples];

///////////Draw summed waveform with 16 channels

int index=nsamples/16;

for(int i=0;i<nsamples/16;i++)
{
	for(int hist=0;hist<16;hist++)
	{hhsin[hist*index+i]=adccsum[i*16+hist];
	
	//cout << i*16+hist << " " << hist << endl;
	}
}

////////calculate all baselines  //hardcoded value 110!!!! watchout

double basel[16]={0};

for(int hist=0;hist<16;hist++)
	{
	int nchans=0;
	for(int i=1;i<(int)(110);i++)
		{basel[hist]+=hhsin[hist*index+i];	
		nchans++;
		}

	cout << "Hist nr " << hist << " nchans " << nchans << " " << basel[hist]/nchans << endl;  

	for(int i=0;i<nsamples/16;i++)
	  {hhsin[hist*index+i]-=(basel[hist]/nchans);	
	  }
	}

for(int i=0;i<nsamples/16;i++)
	for(int hist=0;hist<16;hist++)
	  {adccsum[i*16+hist]=hhsin[hist*index+i];
	  }
    
 delete[] hhsin;   
    } // end dephasing algorithm



  // Re-bin at 50 ns/sample in order to get something clear
  reducedSamplingFrequency = 50.0;
  int nsamples_rebin = (int) (nsamples*samplingfreq/reducedSamplingFrequency);

  //cout << "Samples: " << nsamples_rebin << endl;
  for (int k=0;k<nsamples_rebin;k++)
    {
      adccsumdown[k] = 0.;
      for (int i=k*(nsamples/nsamples_rebin);i<min((k+1)*(nsamples/nsamples_rebin),nsamples);i++)
	{
	  adccsumdown[k] += adccsum[i]/((float)(nsamples/nsamples_rebin));
	}
    }



  return 0;
}

int dump_files(wep *p) 
{
  int nsamples;
  
  //  float sum;
  
  nsamples = p->nSAMPLES;
  float samplingfreq = p->get_sampling(); //ns/chan
  reducedSamplingFrequency = 50.0;
  int nsamples_rebin = (int) (nsamples*samplingfreq/reducedSamplingFrequency);

  //cout << "Going to compute integrals" << endl;
  //Dump results  
  //Try to calculate the integral 
  vector<float> *integral = new vector<float> ;
  integral->push_back(0.);
  for (int i=1;i<nsamples;i++)
    integral->push_back((*integral)[i-1]+adccsum[i-1]*samplingfreq);

  //cout << "Going to open files" << endl;
  ofstream ofile("out.dat");
  for (int k=0;k<nsamples_rebin;k++)
    {
      ofile << (k+0.5)*reducedSamplingFrequency << " " << adccsumdown[k] << endl;
    }
  ofile.close();
  
  ofstream fullfile("outf.dat");
  for (int k=0;k<nsamples;k++)
    {
      fullfile << (k+0.5)*samplingfreq << " " << adccsum[k] << endl;
    }
  fullfile.close();
  
  ofstream intefile("inte.dat");
  for (int k=0;k<nsamples;k++)
    {
      intefile << (k+0.5)*samplingfreq << " " << (*integral)[k] << endl;
    }
  intefile.close();
	
  
  //Dump single pmts
  /*
  ofstream pmtfile("pmts.dat");
  float sum[7] = {0.,0.,0.,0.};
  for (int k=0;k<nsamples;k++)
    {
      for (int npmt=0;npmt<p->nPMT;npmt++)
	{
	  pmtfile << adcc[k+npmt*nsamples] << " " << sum[npmt] << " ";
	  sum[npmt] += adcc[k+npmt*nsamples];
	}
      pmtfile << endl;
    }
  pmtfile.close();
  */
	
  delete integral;

  return 0;
  
}

/*Main process*/
int process_pulse(wep* p) 
{
  int npmt,nsamples, nevent, nrun;
  float samplingfreq;
  float *pin;
  //  float *pout;
  
//   ofstream testtt("times.txt",ios::app);
//   
// 		  testtt<< p->nEVENT << " ";	  
// 		  
//   for(int i=0; i<(MY_MAX_NPMT/2+0.5); i++) 
//   	{ the_event.cardtimes[i]=cardtimes[i];
// 	cout << "---cards " << i << " " << cardtimes[i]<<" " <<the_event.cardtimes[i] << endl;
// 	testtt << cardtimes[i]<<" ";
// 	}
// 	testtt << endl;
// 	testtt.close();	
  //pin = p->data;
  pin = adcc;
  nsamples = p->nSAMPLES;
  npmt = p->nPMT;
  cout << "nsamples: " << nsamples << "; PMTs: " << npmt << endl;
  nevent = p->nEVENT;
  nrun = p->nRUN;
  samplingfreq = p->get_sampling(); //ns/chan
  cout << "Sampling frequency: " << p->get_sampling() << endl;
 
  if (samplingfreq < 0.01) 
    samplingfreq = 1.; //1 ns/chan

  the_event.nn = npmt;

  cout << "### Process event: " << nevent << endl;
  
  //float pretrigger = 4947.;
  // the -100 is to return to the preset value of 1900 for evts with prtrigger of 2000ns. Andrzej
 
   cout << " pretrig"  <<  p->preTRIG << endl;

  float pretrigger=0;
  if(!p->is_CAEN)
    pretrigger = (float)(p->preTRIG-100.);
  else
    pretrigger = (float)(p->preTRIG+100.);	
       
   if(pretrigger<0)
	         pretrigger=fabs(pretrigger);
 
  cout << "pretrigger:: " << pretrigger << endl; 

  //here I need the pretrigger time as well. 
  sum_pmt(p);

  //Now get threshold
  threshold = get_threshold(adccsum,nsamples,samplingfreq,pretrigger);

   cout << " het Threshold " << std::endl;

  int nsamples_rebin = (int) (nsamples*samplingfreq/reducedSamplingFrequency);

  float dtres = threshold /(sqrt((float)(nsamples/nsamples_rebin)));

  //Ok, now look for primary
  //Look for the primary only in the vicinity of the pre-trigger 
  int isrcf = (int) (max((pretrigger-500.),0.)/reducedSamplingFrequency);

  bool primaryfound = false;
  bool secondaryfound = false;
  //1) search for the first point over threshold (onset of the primary)
  // it cannot be at the very end (last 1 mus) of the window! 
  int ionset_primary = 0;
  int ionset_secondary = 0;
  bool primarybool = false;
  int nChannelsSec = (int) (1500./reducedSamplingFrequency);
  for (int i=isrcf; i<(nsamples_rebin-isrcf) && ionset_primary==0;i++)
    {
      if (adccsumdown[i] > dtres) 
	{
	  int chanInRealSampling = (int) (i*reducedSamplingFrequency/samplingfreq);
	  if (!IsASinglePhotoElectron(adccsum,nsamples,samplingfreq,chanInRealSampling,threshold))
	    {
	      primarybool = IsAPrimary(adccsumdown,nsamples_rebin,reducedSamplingFrequency,i,dtres);
	      if (primarybool) 
		{
		  ionset_primary=i;
		  primaryfound=true;		  
		}	      
	    }
	}
    }

  //if (primaryfound) cout << "Primario trovato a " << ionset_primary*reducedSamplingFrequency << endl;
  
  //If not, look for the real secondary. If a secondary is <900 ns from the primary,
  //then the primary is not a real primary!
  if (!secondaryfound)
    {
      int isrcf = (int) (900./reducedSamplingFrequency);
      //1) search for the first point over threshold (onset of the primary)
      for (int i=ionset_primary+isrcf; i<nsamples_rebin && !secondaryfound;i++)
	{
	  if (adccsumdown[i] > dtres) 
	    {
	      if ((i-ionset_primary)*reducedSamplingFrequency < 3000)
		nChannelsSec= (int) (1700./reducedSamplingFrequency);
	      else
		nChannelsSec= (int) (1500./reducedSamplingFrequency); 
	      ionset_secondary = i;	
	      if (IsASecondary(adccsumdown,nChannelsSec,ionset_secondary,dtres,nsamples_rebin,ionset_primary))
		{
		  secondaryfound = true; //it is a real secondary
		  if ((i-ionset_primary)*reducedSamplingFrequency < 900)
		    primaryfound = false; //too close to the secondary to be a real primary! 		    
		    
		}	      
	    }
	}
    }

  
  if (primaryfound)
    {
      cout << "Primary pulse (first guess) found at: " << 
  	(ionset_primary+0.5)*reducedSamplingFrequency << " ns" << endl;
   }
  
  float firstSecTime = 0.0;
  float lastSecTime = 0.0;
  vector<float> secondaryTime;
  vector<float> secondaryAmplitude;
  vector<float> furtherSecondaries;

  float totalSecondaryAmplitudes = 0;
  int largestSecondary = 0;
    

  if (secondaryfound)
    {
      //The time of the primary is also passed so that it can be used to "veto" this region
      furtherSecondaries = LookForOtherSecondaries(adccsumdown,nsamples_rebin,
						   reducedSamplingFrequency,dtres,
						   ionset_secondary,
						   ionset_primary*reducedSamplingFrequency);
						   
	cout << "CHECKING:  secondary or Time, further, after look for other " << secondaryfound << " " << secondaryTime.size() << " " << furtherSecondaries.size() << endl; 
      //Re-order the times of the secondaries
      sort(furtherSecondaries.begin(),furtherSecondaries.end());
      
      vector<float> secondaryAmplitudePreliminaryFirst = GetAmplitudesOfSecondaries(adccsumdown,nsamples_rebin,
									       reducedSamplingFrequency,
									       furtherSecondaries,
									       dtres,ionset_primary);
									       
      cout << "CHECKING:  secondary or Time, further, amplitudes, after look for other " << secondaryfound << " " << secondaryTime.size() << " " << furtherSecondaries.size() << " " << secondaryAmplitudePreliminaryFirst.size() << endl; 
    										
									       
      //Filter non-valid secondaries
      for (size_t j=0;j<furtherSecondaries.size();j++)
	{
	  cout << " IN LOOP " << furtherSecondaries[j] << " " << secondaryAmplitudePreliminaryFirst[j] << endl;
	  if (secondaryAmplitudePreliminaryFirst[j] > 0)
	    {
	      secondaryTime.push_back(furtherSecondaries[j]);
	      secondaryAmplitude.push_back(secondaryAmplitudePreliminaryFirst[j]);
	    }
	}

cout << "CHECKING:  secondary or Time " << secondaryfound << " " << secondaryTime.size() << endl;

      if (secondaryTime.size())
	{
	  firstSecTime = secondaryTime[0]-reducedSamplingFrequency; //cut one bin, to be sure
	  lastSecTime = secondaryTime[secondaryTime.size()-1];
	}
    }

  //Ok, now I have determined the onset, so I have to reprocess the signals and subtract a more 
  //precise baseline where the signal is not there.


  //At this point, we have to revert to the original signal, subtract a running baseline 
  //(where the signal is not there) and calculate integrals.
  
  if (p->use_refine_baseline())
    {
      cout << "Going to refine baseline" << endl;
      RefineBaselineAndSum(p,ionset_primary*reducedSamplingFrequency,firstSecTime,lastSecTime,pretrigger);
    }
  else
    cout << "WARNING: baseline is NOT refined" << endl;


  bool furtherProcessSecondary = true;
  if (!secondaryfound || !secondaryTime.size())
  {furtherProcessSecondary = false;
   cout << "WARNING: not processing cause secondary or Time" << secondaryfound << " " << secondaryTime.size() << endl;
  }
   if (the_event.ov)
     {
       furtherProcessSecondary = false;
       cout << "S2 not calculated because pulse is saturated" << endl;
     }

  if (furtherProcessSecondary)
    {
      vector<float> secondaryAmplitudePreliminary = GetAmplitudesOfSecondaries(adccsumdown,nsamples_rebin,
									       reducedSamplingFrequency,
									       furtherSecondaries,
									       dtres,ionset_primary);

      secondaryTime.clear();
      secondaryAmplitude.clear();
      
      //Filter non-valid secondaries
      for (size_t j=0;j<furtherSecondaries.size();j++)
	{
	  if (secondaryAmplitudePreliminary[j] > 0)
	    {
	      secondaryTime.push_back(furtherSecondaries[j]);
	      secondaryAmplitude.push_back(secondaryAmplitudePreliminary[j]);
	    }
	}

      cout << "Found: " << secondaryTime.size() << " secondaries" << endl;

      ///////added if condition as a test, Andrzej
      if(secondaryTime.size())
	{
	  
	  float maxAmplitudeSecondary = 0.0;
	  for (size_t j=0;j<secondaryTime.size();j++)
	    {
	      if (secondaryAmplitude[j] > maxAmplitudeSecondary)
		{
		  maxAmplitudeSecondary = secondaryAmplitude[j];
		  largestSecondary = j;
		}
	      //totalSecondaryAmplitudes += secondaryAmplitude[j];
	      cout << "Secondary #" << j+1 << " " << secondaryTime[j] <<
		" ns; Approx amplitude: " << secondaryAmplitude[j] << endl;	    
	    }

	  vector<float> singlePESecondaries = GetAmplitudesOfSecondariesInThePMTs(p,secondaryTime,largestSecondary);
	  for (int ipmt=0;ipmt<npmt;ipmt++)
	    {
	      if (p->is_ser_found)
		singlePESecondaries[ipmt] /= p->PMTSer[ipmt];
	      the_event.ss[ipmt] = singlePESecondaries[ipmt];
	      totalSecondaryAmplitudes += singlePESecondaries[ipmt];
	      cout << "Secondary PMT" << ipmt+1 << " = " << singlePESecondaries[ipmt] << endl;
	    }
	  cout << "Total S2: " << totalSecondaryAmplitudes << endl;
	  the_event.s2=totalSecondaryAmplitudes;
	  the_event.nsec = secondaryTime.size();
	  the_event.t2=(int) secondaryTime[0];
	  the_event.t2largest = (int) secondaryTime[largestSecondary];
	  the_event.t2last= (int) secondaryTime[secondaryTime.size()-1];
	  firstSecTime = secondaryTime[0]-reducedSamplingFrequency; //cut one bin, to be sure
	}
      else  // if(secondaryTime.size())
	{
	  the_event.t2=0;
	  the_event.t2largest = 0;
	  the_event.t2last = 0;
	  the_event.s2 = 0;
	  the_event.nsec = 0;
	}
    }
  else
    {
      the_event.t2=0;
      the_event.t2largest = 0;
      the_event.t2last = 0;
      the_event.s2 = 0;
      the_event.nsec = 0;
    }

  
  if(!primaryfound)
    {
      //in some cases a primary is NOT found with 50 ns/chan, while it would be 
      //for 2 ns/chan. This is because the pulse amplitude scales with 1/N, while the background scales 
      //as 1/sqrt(N), so the signal/noise is worse. Here maybe one should do the following: if 
      // (!primaryfound), try to look for it again using the full pulse (and maybe a slighly increased 
      // threshold, to avoid getting baselines). In particular, look between 1 mus and the secondary 
      // pulse.
      if (!secondaryfound || (firstSecTime-pretrigger) > 3000.)
	{
	  int newCandidatePrimary = LookAgainForPrimary(adccsum,nsamples,samplingfreq,
							threshold,firstSecTime);
	  if(newCandidatePrimary)
	    {
	      if (IsAPrimary(adccsum,nsamples,samplingfreq,newCandidatePrimary,threshold))
		{
		  cout << "Found a new candidate primary at " <<  newCandidatePrimary*samplingfreq << " ns" << endl;
		  primaryfound=true;
		  cout << "Primary pulse found at: " << 
		    (newCandidatePrimary+0.5)*samplingfreq << " ns" << endl;
		  ionset_primary = (int) (newCandidatePrimary*samplingfreq/reducedSamplingFrequency);
		}
	    }
	}
    }


  vector<float> resultsPrimary;
  if (primaryfound)
    {
      float primaryTimeLeft = max(0,ionset_primary-1)*reducedSamplingFrequency;
      float primaryTimeRight = min(nsamples,ionset_primary+2)*reducedSamplingFrequency;
      resultsPrimary = RefinePrimaryCalculations(adccsum,nsamples,samplingfreq,
						 primaryTimeLeft,primaryTimeRight,
						 firstSecTime,threshold);
    }
  
  float totalS1 = 0.;
  float fastS1 = 0.;
  float primaryTime = 0.;
  float primaryRiseTime = 0.;
  int iChannelsTot = 0;

  bool furtherProcessPrimary = true;
  if (!resultsPrimary.size())
    furtherProcessPrimary = false;
  else
    {
      if (the_event.ov) //it is an overflow
	{
	  int iTimeWindow = (int) (7000./samplingfreq);
	  int iTimePrimary = (int) (resultsPrimary[2]/samplingfreq);
 	  if (the_event.satur_chan > iTimePrimary-1 && 
 	      the_event.satur_chan < iTimePrimary+iTimeWindow)
 	    {
 	      furtherProcessSecondary = false;
 	      cout << "S1 not calculated because pulse is saturated" << endl;
 	    }
	}
    }


  if (furtherProcessPrimary ) //if there is a valid and non-saturated primary: write data
    {
      
      // cout << " $$$$$$$$$ " << p->isSer << endl;
      
      primaryTime = resultsPrimary[2];
      primaryRiseTime = resultsPrimary[3];
      iChannelsTot = (int) resultsPrimary[4];
      int iChannelsFast = (int) resultsPrimary[5];

      //float primaryTimeLeft = max(0,ionset_primary-1)*reducedSamplingFrequency;
      //float primaryTimeRight = min(nsamples,ionset_primary+2)*reducedSamplingFrequency;

      the_event.rtp = primaryRiseTime;
      the_event.s1intetime = iChannelsTot;

      
      CalculateIntegralsForPrimaries(p,primaryTime,
				     firstSecTime);
      
      
      //GetAmplitudesOfPrimariesInThePMTs(p,
      //					primaryTimeLeft,primaryTimeRight,
      //					primaryTime,firstSecTime);

      //Calculate totalS1 and fastS1 using the information from the single PMTs
      //DO NOT SUM noise PMTs
      for (int ipmt=0;ipmt<npmt;ipmt++)
	{
	  if (!the_event.isnoise[ipmt])
	    {
	      totalS1 += the_event.sp[ipmt];
	      fastS1 += the_event.spf[ipmt]; 
	    }
	}
     
      cout << "Primary amplitude total: "  <<  totalS1 << " (" << iChannelsTot << " channels)" << endl;
      cout << "Primary amplitude fast (80 ns): "  <<  fastS1 << " (" << iChannelsFast << 
	" channels), rise time: " << primaryRiseTime << endl;
      float maxRatio = 0.0;
      int ichanMaxRatio = -1;
      
 
      
      if (totalS1)
	{
	  
	  float FastToTotal = fastS1/totalS1;
	  cout << "Fast to total: " << FastToTotal << endl;
	 
	  for (int ipmt=0;ipmt<npmt;ipmt++)
	    {
	      if (!the_event.isnoise[ipmt])
	    	{
		  float ratio = the_event.sp[ipmt]/totalS1;
		  if (ratio > maxRatio)
		    {
		      maxRatio = ratio;
		      ichanMaxRatio = ipmt;
		    }
	  	}
	    }
	}
      the_event.maxS1ratio = maxRatio;
      the_event.PMTofmaxS1ratio = ichanMaxRatio;
      
    }
    else   // added for waveform display and maybe sers
    {
	std::cout << "or else! Calc Intergrals for primaries! " << std::endl;     
// 	    primaryTime = resultsPrimary[2];
//     primaryRiseTime = resultsPrimary[3];
//     iChannelsTot = (int) resultsPrimary[4];
//     int iChannelsFast = (int) resultsPrimary[5];
   
     // float primaryTimeLeft = max(0,ionset_primary-1)*reducedSamplingFrequency;
      //float primaryTimeRight = min(nsamples,ionset_primary+2)*reducedSamplingFrequency;

    //the_event.rtp = primaryRiseTime;
    //the_event.s1intetime = iChannelsTot;
	    if(p->isSer || p->isGUI || p->isAverage)
		  {CalculateIntegralsForPrimaries(p,pretrigger-50,0);
		
	    
		  }
          
	    }

  

  the_event.s1=totalS1;
  the_event.t1=(int) primaryTime;
  the_event.tm = primaryTime;
  the_event.sf = fastS1;

  //Calculate barx, bary
  //but only if SERS are provided... otherwise write zero
  if (p->is_ser_found)
    {
	float BARX = 0;
	float BARY = 0;
	pmt_array_coord();
	//Better to add a condition like sp[impt]>0?
	if(the_event.s1 > 0)
	{
	for (int ipmt=0;ipmt<npmt;ipmt++){
		if (p->isPMTOn[ipmt])
		{
			BARX += the_event.sp[ipmt]*x_pmt[ipmt]*((1*is_three_inches(ipmt))+9./4.*!(is_three_inches(ipmt)));
			BARY += the_event.sp[ipmt]*y_pmt[ipmt]*((1*(int)(is_three_inches(ipmt)))+9./4.*(int)(!(is_three_inches(ipmt))));
			if (p->debug)
			{
			cout << "ipmt=" << ipmt << ", x_pmt[ipmt]=" << x_pmt[ipmt] << ", int1="<< (int)(is_three_inches(ipmt)) << ", int2="<< (int)(!(is_three_inches(ipmt))) <<", sp=" <<
			the_event.sp[ipmt] << endl;
			cout << "ipmt=" << ipmt << ", y_pmt[ipmt]=" << y_pmt[ipmt] << ", int1="<< (int)(is_three_inches(ipmt)) << ", int2="<< (int)(!(is_three_inches(ipmt))) <<", sp=" <<
			the_event.sp[ipmt] << endl;
			}
	
		  }
		}
		the_event.barx = BARX/the_event.s1;
	      	the_event.bary = BARY/the_event.s1;
		if (p->debug)
		{
		  cout << "BARX = " << the_event.barx << endl;
		  cout << "BARY = " << the_event.bary << endl;
		}
	}
    }
  return 0;
}
    

int get_singlephe(wep* p)
{
  //  int npmt, nsamples, nevent, nrun;
  int nsamples;
  float samplingfreq;
//   float *pin;
//   float *pout;
  
  nsamples = p->nSAMPLES;
  samplingfreq = p->get_sampling(); //ns/chan
  
  int nOfFoundPulses = 0;
 
  phe_peak.clear();

  int ionset_primary = 0;
  float primarytime = (float) the_event.t1;

  ionset_primary = (int) ((primarytime/samplingfreq)-0.5);

  if (ionset_primary > 0)
    {
      phe_peak.push_back(ionset_primary);
      nOfFoundPulses++;
    }
  else
    return 0; //no primary found
      
  //Here one can count one-by-one p.e.

  //the search should stop at the onset of the secondary
  //it may be less than 12 mus after the primary
  //Look for other peaks above threshold      
  //we must stop somewhat before the secondary, to avoid counting spurious peaks

  float mindistance;
  bool foundpulse=false;
  int maxWindowForSinglePE = (int) (12000./samplingfreq);
  
  int secChannel = (int) ((the_event.t2-50)/samplingfreq);
  if (!secChannel) secChannel = nsamples;
  
  int startOfSecWindow = secChannel - (int) (1000./samplingfreq);
  do{
    foundpulse = false;
    if (nOfFoundPulses==1) 
      mindistance = (int) (25./samplingfreq); //longer window after the very first pulse
    else
      mindistance = (int) (10./samplingfreq);
    for (int i=(int) phe_peak[nOfFoundPulses-1];
	 i<min(startOfSecWindow,(int) (phe_peak[nOfFoundPulses-1]+maxWindowForSinglePE)) && !foundpulse ;i++)
      {

	if ((adccsum[i] > threshold) && i > ((int) phe_peak[nOfFoundPulses-1]+mindistance)) 
	  {
	    if (i > phe_peak[0]+ (int)(100./samplingfreq) )
	      {
		phe_peak.push_back((float) i);
		nOfFoundPulses++;
		foundpulse = true;
	      }
	  }
      }
  }while(foundpulse && nOfFoundPulses<MY_MAX_PEAKS);

  cout << "Found: " << nOfFoundPulses << " single p.e. in the primary" << endl;
  for (int i=0;i<nOfFoundPulses;i++)
    {
      //Here convert phe_peak from channel to ns
      phe_peak[i] = (phe_peak[i]+0.5)*samplingfreq;
      cout << i+1 << "-th peak: " << phe_peak[i] << " ns" << endl;
      
      /*
      std::ofstream ooo("phe.dat",ios::app);
      if (i>0)
	ooo << phe_peak[i][0]-phe_peak[0][0] << endl;
      ooo.close();
      */
    }

  /*
  int numberOfValidPeaks = 0;
  float thePhe = 0.;

  //Find the average phe level
  if (nOfFoundPulses > 1)
    {     
      int theWindowLeft = (int)(5./samplingfreq); //2 ns before 
      int theWindowRight = (int) (6./samplingfreq);
      for (int npeak=1;npeak<nOfFoundPulses-1;npeak++)
	{
	  if (phe_peak[npeak]-phe_peak[0] > (200./samplingfreq)) //far from the main
	    if (phe_peak[npeak]-phe_peak[npeak-1] > (10./samplingfreq) &&  //far from the previous
		phe_peak[npeak+1]-phe_peak[npeak] > (15./samplingfreq)) //far from the last one
	      {
		float sum = 0;
		for (int i=std::max(0,(int)(phe_peak[npeak]-theWindowLeft));
		     i<std::min((int)(phe_peak[npeak]+theWindowRight),nsamples);i++)
		  {
		    sum += adccsum[i]*samplingfreq;
		    //cout << "Window " << npeak << " " << std::max(0,(int)(phe_peak[npeak]-theWindowLeft)) << 
		    //  " " << std::min((int)(phe_peak[npeak]+theWindowRight),nsamples) << endl;
		  }
		thePhe += sum;
		numberOfValidPeaks++;
	      } 
	}
      //add the area of the last peak
      if (phe_peak[nOfFoundPulses]-phe_peak[nOfFoundPulses-1] > (10./samplingfreq))
	{
	  float sum = 0;
	  for (int i=std::max(0,(int)(phe_peak[nOfFoundPulses]-theWindowLeft));
	       i<std::min((int)(phe_peak[nOfFoundPulses]+theWindowRight),nsamples);i++)
	    sum += adccsum[i]*samplingfreq;
	  thePhe += sum;
	  numberOfValidPeaks++;
	}
    }
    
  
  if (numberOfValidPeaks)
    {
      thePhe = thePhe/((float)numberOfValidPeaks);
      cout << "Photoelectron level: " << thePhe << endl;
    }

  */

  return 0;
}


vector<float> GetAmplitudesOfSecondariesInThePMTs(wep* p,vector<float> secPositions,int maxSecondary)
{
  int nsamples = p->nSAMPLES;
  float samplingfreq = p->get_sampling(); //ns/chan
  int npmt = p->nPMT;

  //This is calculated using the original sampling frequency
  int timeWindow = (int) (30000./samplingfreq); //calculate secondary amplitude over 30 mus
  vector<float> amplitude;
  float appoggio = 0.0;

  int iChannelsTot = 0;
  //Loop over the secondaries
  for (size_t ipmt=0;ipmt< ((size_t) npmt);ipmt++)
    {
      if (p->isPMTOn[ipmt])
	{
	  appoggio = 0.0;
	  for (size_t i=0;i<secPositions.size();i++)
	    {
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
		  appoggio += adcc[j+ipmt*nsamples]*samplingfreq; 
		  iChannelsTot++;
		}
	    }
	  amplitude.push_back(appoggio);
	}
      else
	amplitude.push_back(0.);
    }
  

  //Get the rise time (rise time of the largest secondary)
  float areaOfLargestSecondary = 0.0;

  for (size_t ipmt=0;ipmt < ((size_t) npmt);ipmt++)
    {
      iChannelsTot = 0;
      int startingChan = (int) (secPositions[maxSecondary]/samplingfreq);
      int finalChan = startingChan+timeWindow;
      if (maxSecondary != (int) (secPositions.size()-1))
	{
	  int nextSecondary = (int) (secPositions[maxSecondary+1]/samplingfreq);
	  finalChan = min(startingChan+timeWindow,nextSecondary);
	}
      
      for (int j=startingChan;j<min(finalChan,nsamples);j++)
	areaOfLargestSecondary += adcc[j+ipmt*nsamples]*samplingfreq; 
    }

  //cout << "Area of largest secondary: " << areaOfLargestSecondary << endl;

  //Now I have to determine the rise-time 0-90%
  float risetimethreshold = 0.9*areaOfLargestSecondary;
  float runningrisetime = 0.0;
  
  int startingChan = (int) (secPositions[maxSecondary]/samplingfreq);
  int finalChan = startingChan+timeWindow;
  if (maxSecondary != (int) (secPositions.size()-1))
    {
      int nextSecondary = (int) (secPositions[maxSecondary+1]/samplingfreq);
      finalChan = min(startingChan+timeWindow,nextSecondary);
    }
  float theRiseTime = 0;
  int iRiseTime = startingChan;
  for (int j=startingChan;j<min(finalChan,nsamples) && runningrisetime<risetimethreshold;j++)
    {
      runningrisetime += adccsum[j]*samplingfreq; 
      iRiseTime = j;
    }
  theRiseTime = (iRiseTime-startingChan)*samplingfreq;
  //
  cout << "Rise time of secondary: " << theRiseTime << endl;
  the_event.rts = theRiseTime;

  /*
  ofstream ffff("test1.out");
  for (int j=0;j<nsamples;j++)
    ffff << adcc[j] << " " << adcc[j+nsamples] << " " << adcc[j+2*nsamples] << " " << adcc[j+3*nsamples] << endl;
  ffff.close();
  */

  return amplitude;
}

void GetAmplitudesOfPrimariesInThePMTs(wep* p,
				       float primaryTimeLeft,float primaryTimeRight,
				       float primaryTime, //onset time based on the summed signal
				       float firstSecTime)
{
  int nsamples = p->nSAMPLES;
  float samplingfreq = p->get_sampling(); //ns/chan
  int npmt = p->nPMT;
  //This is calculated using the original sampling frequency
  
  vector<float> amplitude;
  
  for (int ipmt=0;ipmt<npmt;ipmt++)
    {
      int ionset_primary = 0;
 
      int ichanL = (int) (primaryTimeLeft/samplingfreq);
      int ichanR = (int) (primaryTimeRight/samplingfreq);
      for (int i=ichanL; i<ichanR && ionset_primary==0;i++)
	{
	  if (adcc[i+ipmt*nsamples] > 0.7*threshold) 
	    ionset_primary=i;
	}
      //If primary onset not found, iterate
      if (ionset_primary == 0)
	{
	  for (int i=ichanL; i<ichanR && ionset_primary==0;i++)
	    {
	      if (adcc[i+ipmt*nsamples] > 0.7*threshold/2.) 
		ionset_primary=i;
	    }
	}

      the_event.tp[ipmt] = ionset_primary; //this is just for information:
      //it may fail for certain PMTs if the signal is too small. 

      //Use the onset based on the summed signal to get the integrals.
      int scanbackChannels = 10; //go back 10 ns before the onset.
      ionset_primary = (int) primaryTime - scanbackChannels;
      if (ionset_primary < 0) ionset_primary = 0;

       //Integrate the primary signal for 7 mus, if the secondary is not there
      float amplitudeFull = 0.0;
      float amplitudeFast = 0.0;
      int timeWindow = (int) (7000./samplingfreq);
      int secChannel = (int) (firstSecTime/samplingfreq);
      if (!secChannel) secChannel = nsamples;
      //integrate up to 0.5 mus from the secondary
      int timeWindowBeforeSecondary = (int) (500./samplingfreq);
      int integrationChannels = 0;
      if (ionset_primary)
	{
	  for (int i=ionset_primary;i<min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow);i++)
	    {
	      amplitudeFull += adcc[i+ipmt*nsamples]*samplingfreq; 
	      integrationChannels++;
	      //fast component, up to 80 ns
	      if ((i-ionset_primary)*samplingfreq < 80.0) 
		{
		  amplitudeFast += adcc[i+ipmt*nsamples]*samplingfreq; 
		}
	    } 

	  if (p->is_ser_found)
	    {
	      amplitudeFull /= p->PMTSer[ipmt];
	      amplitudeFast /= p->PMTSer[ipmt];
	    }

	  the_event.sp[ipmt] = amplitudeFull;
	  the_event.spf[ipmt] = amplitudeFast;
	}
      else
	{
	  the_event.sp[ipmt] = 0.;
	  the_event.spf[ipmt] = 0.0;
	}
      cout << "Primary: PMT#" << ipmt+1 << " " << the_event.tp[ipmt] << " " << the_event.sp[ipmt] << " " << 
	the_event.spf[ipmt] << endl;
      //      if(p->isPMTOn[ipmt]) the_event.s1intetime = integrationChannels*samplingfreq;
    }
}

void RefineBaselineAndSum(wep* p,float primTime, float firstSec, float lastSec,float pretrigger)
{
  const int nst = p->nSAMPLES;
  int npmt = p->nPMT;
  float samplingfreq = p->get_sampling(); //ns/chan

  for (int ipmt=0;ipmt<npmt;ipmt++)
    rmsOfPmts[ipmt] = 0.;

std::cout << " ^^^ pretrigger in refine baseline" << pretrigger << std::endl;
  //1) Revert to the original signal
  int *pin = p->data;

  for (int ipmt=0; ipmt<npmt;ipmt++)
    {
      if (p->isPMTOn[ipmt]) //take only active PMTs
	{
	  int nstart= (int) (pretrigger*0.2/samplingfreq);
	  int nstop = (int) (pretrigger*0.8/samplingfreq);


	  int histmax, imax; 
	  
	  if(!p->is_CAEN)	  
	    {
	  //Get first baseline
		int hist[256];

		for(int i=0; i<256; i++) hist[i]=0;
		for(int is=nstart; is<nstop; is++) hist[pin[idx[ipmt]+is]+128]++;
		histmax = 0; imax = 0;
		for(int i=0; i<256; i++) {
		    if(hist[i]>histmax) {
			imax=i-128;
			histmax=hist[i];
			}
		    }
	      }
	   else
	   {

           //Get first baseline
	  int hist[1024];
	  
	  for(int i=0; i<1023; i++) hist[i]=0;
std::cout << "++RefineBaseline nstart, nstop" << nstart << " " << nstop << " idx[ipmt] " << idx[ipmt] << " " << pin[idx[ipmt]] << std::endl;
	  for(int is=nstart; is<nstop; is++) hist[pin[idx[ipmt]+is]]++;
	  histmax = 0; imax = 0;
	  for(int i=0; i<1023; i++) {
	    if(hist[i]>histmax) {
	      imax=i;
	      histmax=hist[i];
		  }
		}
	   } 


	  float sum=0.0;
	  float sum2=0.0;
	  int maxchan = pin[idx[ipmt]+nstart];
	  int minchan = pin[idx[ipmt]+nstart];
	  for(int is=nstart; is<nstop; is++) 
	    {
	      sum += pin[idx[ipmt]+is]-((float) imax);
	      sum2 += (pin[idx[ipmt]+is]-((float) imax))*(pin[idx[ipmt]+is]-((float) imax));
	      if (pin[idx[ipmt]+is] > maxchan) maxchan = pin[idx[ipmt]+is];
	      if (pin[idx[ipmt]+is] < minchan) minchan = pin[idx[ipmt]+is];
	    }
	  sum /= (float)(nstop-nstart);
	  sum += (float) imax;
	  float sigma=sqrt((sum2/(float)(nstop-nstart)) - ((sum-imax)*(sum-imax)));
	  //cout << ipmt << " " << sum << " " << sigma << endl;
	  rmsOfPmts[ipmt] = sigma;

	  //Subtract
	  for(int i=0; i<nst; i++) 
	    adcc[idx[ipmt]+i] = sum - (float) pin[idx[ipmt]+i];
	  the_event.baseline[ipmt] = sum;
	  the_event.ppnoise[ipmt] = (float)(maxchan-minchan);
	  the_event.noise[ipmt] = rmsOfPmts[ipmt];
	}
     
    }

  //2) Calculate the running baseline
  int nn = 0;
  float sum,lbase;
  int index = 0;
  float windowPrimary = 7100; //2000; // original: 7100.;
  float windowSecondary = 30100.;
  float windowBeforeSecondary = 6000.;
  if (firstSec < 15000) windowSecondary=40000.;

  //Number of samples for the average  

  const int smplSum = 256;

  const int maxIndexForBase = nst/smplSum + 1;

  //it is dimensioned up to the number of samples to be
  //used for the baseline subtraction
  float prop_base[maxIndexForBase]; 

  //Main loop on PMTs 
  for (int ipmt=0;ipmt<npmt;ipmt++)
    {
      if (p->isPMTOn[ipmt]) //take only active PMTs
	{
	  //Initialize the vector 
	  for (int i=0;i<maxIndexForBase;i++)
	    prop_base[i] = -10000.;
	  
	  //
	  //  local baseline evaluation
	  int it;
	  for(it=0; it<nst-smplSum; it+=smplSum) 
	    {
	      sum = 0.0; nn=0;
	      for(int iv=0; iv<smplSum; iv++) {
		nn++; 
		sum+=adcc[(long int)(it+iv+ipmt*nst)];
	      }
	      
	      if(nn>0)
		{
		  lbase = sum/((float) nn);
		  //cout << ipmt << " " << it << " " << sum << " " << nn << " " << lbase << endl;
		  //cout << primTime << " " << firstSec << " " << lastSec << endl;
		  index = it/smplSum;
		  //cout << index << endl;
		  
		  //check if the current bin (it-->it+256) contains signal
		  //primary and secondary found
		  if (primTime > 0 && firstSec > 0)
		    {
		      
		      
		      //pretrigger
		      if ((it+smplSum)*samplingfreq < primTime && (it+smplSum)*samplingfreq < (firstSec-windowBeforeSecondary))
			prop_base[index]=lbase;
		      //between primary and secondary
		      if (it*samplingfreq > (primTime+windowPrimary) &&  (it+smplSum)*samplingfreq < (firstSec-windowBeforeSecondary))
			prop_base[index]=lbase;
		      //after secondary	    
		      float endOfWindow = lastSec+windowSecondary;
		      if (it*samplingfreq > (primTime+windowPrimary) && it*samplingfreq > endOfWindow)
			prop_base[index]=lbase;
		      
		      
		    }
		  //particular cases
		  //only secondary
		  else if (primTime <= 0 && firstSec > 0)
		    {
		      //before secondary
		      if ((it+smplSum)*samplingfreq < firstSec)
			prop_base[index]=lbase;
		      //after secondary
		      float endOfWindow = lastSec+windowSecondary;
		      if (it*samplingfreq > endOfWindow)
			prop_base[index]=lbase;
		      
		      
		    }
		  //only primary
		  else if (primTime > 0 && firstSec <= 0)
		    {
		      //before primary
		      if ((it+smplSum)*samplingfreq < primTime)
			prop_base[index]=lbase;
		      //after primary
		      if (it*samplingfreq > (primTime+windowPrimary))
			prop_base[index]=lbase;
		      
		     
		    }
		  //none of the two (only baseline)
		  else
		    {
		      prop_base[index]=lbase;
		      
		     
		    }
		    
		}
	    }
	  
	  //Complete the vector
	  for (int ll=index;ll<(maxIndexForBase-1);ll++)
	    prop_base[ll+1] = prop_base[index];
	  
	  //for (int ij=0;ij<maxIndexForBase;ij++)
	  //	cout << ipmt << " " << ij << " " << ij*smplSum << " " << prop_base[ij] << endl;
	  
	  float Sx = 0;
	  float Sy = 0;
	  int npoints = 0;
	  float Sx_2 = 0;
	  float Sxy = 0;
	  float slope = 0;
	  float intercept = 0;
	  
	  /*
	  //Idea: pre-fit, only with pre-trigger and last 20 mus. Then accept other points in the fit only if
	  //they are ok! 
	  for (int ik=0;ik<maxIndexForBase;ik++)
	  {
	  if (prop_base[ik] > -1000.)
	  if (ik*smplSum < primTime || (maxIndexForBase-ik)*smplSum<20000.)
	  {
	  npoints++;
	  Sx += (float) ik;
	  Sy += prop_base[ik];
	  Sx_2 += (float) (ik*ik);
	  Sxy += prop_base[ik]*ik;
	  }
	  }
	  if (npoints)
	  {
	  slope = (Sx*Sy-npoints*Sxy)/(Sx*Sx-npoints*Sx_2);
	  intercept = (Sxy*Sx-Sx_2*Sy)/(Sx*Sx-npoints*Sx_2);
	  }
	  
	  cout << "Preliminary fit: " << slope << " " << intercept << endl;
	  float rmsPreliminary = 0.;
	  for (int ik=0;ik<maxIndexForBase;ik++)
	  {
	  if (prop_base[ik] > -1000.)
	  if (ik*smplSum < primTime || (maxIndexForBase-ik)*smplSum<20000.)
	  {
	  rmsPreliminary += (prop_base[ik]-(slope*((float) ik) + intercept))*
	  (prop_base[ik]-(slope*((float) ik) + intercept));
	  }
	  }
	  if (npoints > 1)
	  rmsPreliminary=sqrt(rmsPreliminary/npoints);
	  
	  cout << "rms preliminary: " << rmsPreliminary << endl;
	  //Correct other points
	  for (int ik=0;ik<maxIndexForBase;ik++)
	  {
	  if (prop_base[ik] > -1000.)
	  if (ik*smplSum > primTime && (maxIndexForBase-ik)*smplSum>20000.)
	  {
	  float diff = fabs(prop_base[ik]-(slope*((float) ik) + intercept));
	  if (diff > 3.0*rmsPreliminary) 
	  {
	  prop_base[ik]=-10000.;
	  cout << "Discarded sample: " << ik*smplSum << " " << diff << " " << endl;
	  }
	  }
	  }
	  */
	  
	  //Make a linear fit (least squares method)
	  Sx = 0;
	  Sy = 0;
	  npoints = 0;
	  Sx_2 = 0;
	  Sxy = 0;
	  slope = 0;
	  intercept = 0;
	  for (int ik=0;ik<maxIndexForBase;ik++)
	    {
	      if (prop_base[ik] > -1000.)
		{
		  npoints++;
		  Sx += (float) ik;
		  Sy += prop_base[ik];
		  Sx_2 += (float) (ik*ik);
		  Sxy += prop_base[ik]*ik;
		}
	    }
	  if (npoints && (Sx>0 ))
	    {
	      if((Sx*Sx-npoints*Sx_2))
	      {
		slope = (Sx*Sy-npoints*Sxy)/(Sx*Sx-npoints*Sx_2);
		intercept = (Sxy*Sx-Sx_2*Sy)/(Sx*Sx-npoints*Sx_2);
	      }
	    }
	  else
	    {
	      cout << "Problem in interpolation: no valid points! " << endl;
	    }
          
	  
	  //cout << npoints << endl;
	  //std::ofstream fff("base.dat");
	  for (int ik=0;ik<maxIndexForBase;ik++)
	    {
	      //*When the if here is commented, all samples are corrected (not only those in the 
	      //signal region*//
	      if (prop_base[ik] < -1000.)
		prop_base[ik] = slope*((float) ik) + intercept;
	      
	      //fff << ipmt << " " << ik << " " << ik*smplSum << " " << prop_base[ik] << endl;
	    }
	  //fff.close();
	  
	  
	  for (int ik=0;ik<nst;ik++)
	    {
	      int index =ik/smplSum;
	      if (index >= maxIndexForBase) index = maxIndexForBase-1;
	      adcc[(long int)( ik+ipmt*nst)] = adcc[(long int)(ik+ipmt*nst)] - prop_base[index];
	      
	    }

	}
    }
  

  //Eventually sum the pmts 
  sum_pmt(p);

}

void CalculateIntegralsForPrimaries(wep* p,float primTime, float firstSec)
{
  const int nst = p->nSAMPLES;
  int npmt = p->nPMT;
  float samplingfreq = p->get_sampling(); //ns/chan


std::cout << "In calculate integrals for primaries" << endl;

  //zero the adccsumcorr array
  if(p->isGUI || p->isAverage)
    {
      for(int i=0;i<nst*npmt;i++)
	adccsumcorr[i]=0;	  
      for(int ipmt=0;ipmt<npmt;ipmt++)
	ser_peaks[ipmt].resize(0);	  
    }
  
  
  cout << "++++ primTim" << primTime << endl;
	
	
  int windowLenght = (int) (50./samplingfreq);
  int channelsBefore = (int) (5./samplingfreq); //channels before the peak
  int channelsAfter = (int) (25./samplingfreq);  //channels after the peak 
  float threshold_multiplier=3.0;
  
  if(p->chan_before)
	  channelsBefore= (int)(p->chan_before/samplingfreq);
  
  if(p->chan_after)
	  channelsAfter= (int)(p->chan_after/samplingfreq);
  
  if(p->integr_window)
	  windowLenght= (int)(p->integr_window/samplingfreq);
  
  if((int)(p->threshold+0.5))
	  threshold_multiplier= p->threshold;
  
//threshold_multiplier=4.5;
  cout << "---parameters " << threshold_multiplier << " " << windowLenght << " " <<
   channelsAfter << " " << channelsBefore << endl; 
  
 // cout << " ++++ windows " <<  windowLenght << " " << channelsBefore << " " << channelsAfter << " " <<  threshold_multiplier << endl;

  
  //std::exit(-1);
//     if(p->isSer)
//     {
//       windowLenght = (int) (30./samplingfreq);
//       channelsBefore = (int) (3./samplingfreq); //channels before the peak
//       channelsAfter = (int) (12./samplingfreq); 
//     }

  //cout << "Window Lenght: " << windowLenght << " chan" << endl;

  //Here one has to set the number of channels to be included 
  //in the integral before and after the samples above threshold.
  //"Official" for now: 5 ns before and 25 after
  int minNumberOfSamplesAboveThreshold = 1;
  int nOfScanBackChannels = 100;

  //we start looking for windows to integrate from 100 ns before the 
  //onset calculated on the sum pulse
  int ionset_primary = (int) (primTime/samplingfreq)-nOfScanBackChannels;
  
  int secChannel = (int) (firstSec/samplingfreq);
  if (!secChannel) secChannel = nst;

  int timeWindow=0;
  if(p->isAverage)
  	timeWindow = (int) (15000./samplingfreq);
  else if(p->isSer)
	{timeWindow = (int) (4000./samplingfreq);
         ionset_primary=4000;
	}
  else
	timeWindow = (int) (7000./samplingfreq);
  //integrate up to 0.5 mus from the secondary
  int timeWindowBeforeSecondary = (int) (500./samplingfreq);

std::cout << "++++SER onset:" << ionset_primary << " window: " << timeWindow << std::endl;

  //Get the original data back
  int *pin = p->data;
  float minpmtthreshold = 2.5; // in ADC
#ifdef ML_FOR_PHOTONS
  std::vector<float> singlePheTimeList;
   double s1inwin[MY_MAX_NPMT];
   for (int i=0;i<MY_MAX_NPMT;i++)
     s1inwin[i] = 0.;
   float beginOfWindow = 1500.;
   float endOfWindow = the_event.s1intetime;
#endif

   //ofstream myfile("testSER.dat");

  //Main loop on PMTs 
  for (int ipmt=0;ipmt<npmt;ipmt++)
    {
      if (p->isPMTOn[ipmt]) //take only active PMTs
	{
	  //Luciano: increased threshold from 3*rms to 3.5*rms
	  std::cout << "impt" << ipmt << " ---- rmsOFPMTs: " << rmsOfPmts[ipmt] << " " << threshold_multiplier*rmsOfPmts[ipmt] << std::endl;
	  pmtthreshold[ipmt] = (float)(rmsOfPmts[ipmt]*threshold_multiplier);

	  for (int i=0;i<nst;i++)
	   {		   	   
	     //isAboveThresholdPre[i]=false;
	     isAboveThreshold[(long int)(i+ipmt*nst)] = false;
	   }	  	  
	  //First round. Put bits up. Notice: last sample is always FALSE
	  for (int i=windowLenght;i<nst-1;i++)
	    {
	      if (adcc[(long int)(i+ipmt*nst)] > pmtthreshold[ipmt] && i>windowLenght)
		{
		  //isAboveThresholdPre[i]=true;
		  isAboveThreshold[(long int)(i+ipmt*nst)] = true;
		}
	    }
		
	  //Check if window is ok, and then extend of left/right
	  for (int i=windowLenght;i<nst;i++)
	    {
	      int samplesAboveThreshold = 0;
	      //A window is starting at chan i-th
	      if (!isAboveThreshold[(long int)(i-1+ipmt*nst)] &&  isAboveThreshold[(long int)(i+ipmt*nst)])
		{
		  //counts how many channels until the pulse come below threshold
		  for (int kk=i;isAboveThreshold[(long int)(kk+ipmt*nst)];kk++)
		    samplesAboveThreshold++;
		}

	      //If the window is too short, delete it
	      if (samplesAboveThreshold < minNumberOfSamplesAboveThreshold)
		{
		  for (int kk=i;kk<(i+samplesAboveThreshold);kk++)
		    isAboveThreshold[(long int)(kk+ipmt*nst)] = false;
		}
	      else
		{
		  //now add part on left and right		  
		  int kk = 0;
		  for (kk=i-1;kk>max(0,i-1-channelsBefore);kk--)
		    {
		      //isAboveThresholdPre[kk]=true;
		      isAboveThreshold[(long int)(kk+ipmt*nst)] = true;
		    }		  
		  //part on the right
		  for (kk=i+1;kk<min(i+1+channelsAfter,nst);kk++)
		    {
		      if (!isAboveThreshold[(long int)(kk+ipmt*nst)])
			isAboveThreshold[(long int)(kk+ipmt*nst)] = true;
		    }
		  i = min(kk,nst); //jump at the end of this window
		}
	    }
       

	  //Ok, now merge windows
	  for (int i=windowLenght;i<nst-1;i++)
	    {
	      //one sample is above threshold and the next one is NOT
	      if (isAboveThreshold[(long int)(i+ipmt*nst)] && !isAboveThreshold[(long int)(i+1+ipmt*nst)]) 
		{
		  int nextChanAboveThreshold = nst;
		  bool found=false;
		  //look for next sample above threshold 
		  for (int j=i+1;j<nst && !found;j++)
		    {
		      if (isAboveThreshold[(long int)(j+ipmt*nst)])
			{
			  nextChanAboveThreshold = j;
			  found = true;
			}
		    }
		  //If there is not enough space between pulses, merge windows
		  if ((nextChanAboveThreshold-i) <= windowLenght)
		    {
		      for (int j=i+1;j<=nextChanAboveThreshold;j++)
			isAboveThreshold[(long int)(j+ipmt*nst)]= true;
		    }
		}
	    }
	  /*
	  for (int i=0;i<nst;i++)
	    myfile << ipmt << " " << i << " " << adcc[i+ipmt*nst] << " " << isAboveThreshold[i+ipmt*nst]
		   << " " << pin[idx[ipmt]+i] << endl;
	  */
	  //Ok, now we have the profile.

	  //Calculate baselines before and after each block
	  double s1fast = 0;
	  double s1tot = 0;

	  int firstBlock = 0;
	  int lastBlock = 0;	 
	  
	  //Now calculate integrals
          std::cout <<  " SER Filling "   << ionset_primary << " to: " << min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow) << std::endl;
	  for (int i=ionset_primary;
	       i<min(secChannel-timeWindowBeforeSecondary,ionset_primary+timeWindow);i++)
	    {	     		 
	      //left side
	      float baseleft = 0;
	      float baseright = 0;
	      float integral = 0;
	      float integralfast = 0;
	      int chanstart=0;
	          
	      if(p->isGUI || p->isAverage)
		      adccsumcorr[(long int)(i+ipmt*nst)]=0;
		
	      //added extra condition to correct events that start too early. Andrzej
	      if ((!isAboveThreshold[(long int)(i+ipmt*nst)] && isAboveThreshold[(long int)(i+1+ipmt*nst)]) || 
		  (i==ionset_primary && isAboveThreshold[(long int)(i+ipmt*nst)])) 
		//chan i+1 is the first of the block
		{
		  if (!firstBlock) firstBlock = i+1;
		  
		  chanstart=i+1;
		  int kk = 0;
		  int jj = 0;
		  int nChanLeft = 0;
		       
		  for (kk=i;kk>i-windowLenght;kk--) //go back 50 ns
		    {
		      baseleft += pin[idx[ipmt]+kk];
		      nChanLeft++;
		    }
		  baseleft /= ((float) (nChanLeft));
		  //cout << " **** Start **** " << i << endl;
		  //cout << "baseleft " << baseleft << " " << nChanLeft << endl;

		  //calculate *main* integral 
		  int ichanint = 0;
		  int ichanintfast = 0;
		  float integralwindow = 0;
		  int ichanintwindow = 0;
		  float maxinwindow = -1;
		  for (kk=i+1;isAboveThreshold[(long int)(kk+ipmt*nst)];kk++)
		    {
		      //ionset_primary+nOfScanBackChannels corresponds to the t1
		      //calculated on the sum pulse
		      //Notice that the first integration (kk loop) may start 
		      //*before* the actual t1. In this case, take as "fast", 
		      //what is -20 ns < t1 < 80 ns
		      int itime = kk-(ionset_primary+nOfScanBackChannels); //this can be negative
		      int fastWindowLeft = (int) ((-20.0)/samplingfreq); //-20 ns
		      int fastWindowRight = (int) (80./samplingfreq); //80 ns		 
		      if (fastWindowLeft < itime && itime < fastWindowRight)
			{
			  integralfast += (baseleft-pin[idx[ipmt]+kk]); //revert the pulse
			  ichanintfast++;
			}
		      if (itime > beginOfWindow && itime < endOfWindow) 
			{
			  integralwindow += (baseleft-pin[idx[ipmt]+kk]); //revert the pulse
			  ichanintwindow ++;
			}
		      integral += (baseleft-pin[idx[ipmt]+kk]);
		     
		      ichanint++;
		      
		      //for display purposes
		      if(p->isGUI || p-> isAverage)
		      adccsumcorr[(long int)(kk+ipmt*nst)]=(baseleft-pin[idx[ipmt]+kk]) ;
		      
		      
		      
		      if (baseleft-pin[idx[ipmt]+kk] > maxinwindow)
			maxinwindow = baseleft-pin[idx[ipmt]+kk];
		    }
		  //cout << "integral " << integral << " " << ichanint << endl;

		  //calculate baseright
		  int nchanRight = 0;
		  for (jj=kk+1;jj<kk+1+windowLenght;jj++)
		    {
		      baseright += (baseleft-pin[idx[ipmt]+jj]);
		      nchanRight++;
		    }
		  baseright /= ((float) nchanRight);

		  //cout << "baseright " << baseright << " " << nchanRight << endl;
		  //ok, now correct integrals for baselines
		  float correction = 0.5*baseright*ichanint;
		  //Notice: the original pulse is NEGATIVE!
		  //myfile << ipmt << " " << (integral-correction) << " " <<  
		  // i << " " << ichanint << " " << maxinwindow << endl;
		  if(p->isGUI)
		  {
			  peak tt;
			  tt.tstart=chanstart;
			  tt.tstop=kk;
			  tt.integ=integral-correction;
			  tt.amp=maxinwindow;
			  tt.length=ichanint;
			  tt.npmt=ipmt;
			  
			  ser_peaks[ipmt].push_back(tt);
			  
			  }
		  
		     
			  	  
		  if(p->isSer)
		  {
		  		    
			  my_peak.tstart=chanstart;
			  my_peak.tstop=kk;
			  my_peak.integ=integral-correction;
			  my_peak.amp=maxinwindow;
			  my_peak.length=ichanint;
			  my_peak.npmt=ipmt;	  
			  
			 ser_tree->Fill();  
		  //if(noise==false)
		  //{	 
	        	ser_hist[ipmt]->Fill(integral-correction);
			ser_leng[ipmt]->Fill(ichanint);
			ser_amp[ipmt]->Fill(maxinwindow);
		  //}	
		  //if(noise==true)
		  //ser_noise[ipmt]->Fill(integral-correction);
		  
		  }
		    //subtract correction to integral
		  if(p->isGUI || p->isAverage)
		 	{ for(int kk=i+1;isAboveThreshold[(long int)(kk+ipmt*nst)];kk++)
				{
			  	adccsumcorr[(long int)(kk+ipmt*nst)]-=0.5*baseright;
		  		}
		  	}	 		  
		  //
#ifdef ML_FOR_PHOTONS
		  if ((i*samplingfreq-primTime) > beginOfWindow &&  
		      ichanint < (channelsBefore+channelsAfter+10) && 
		      (i*samplingfreq-primTime) < endOfWindow)
		    singlePheTimeList.push_back(i*samplingfreq-primTime);
#endif
		  //              if (!p->isGUI && !p->isSer && noise==false )
                  if (!p->isGUI && !p->isSer )
		    {
		      if (p->is_ser_found && integral-correction >= p->PMTValley[ipmt])
			s1tot += (integral-correction);
		    }
		  else
		    s1tot += (integral-correction);
		      
		  if (ichanintfast)
		    {
		      float correctionfast = 0.5*baseright*ichanintfast;
		      //              if (!p->isGUI && !p->isSer && noise==false )
		      if (!p->isGUI && !p->isSer )
			{
			  if (p->is_ser_found && integralfast - correctionfast >= p->PMTValley[ipmt])
			    s1fast += (integralfast - correctionfast);
			}
		      else
			s1fast += (integralfast - correctionfast);
		    }
		  //Flag to protect against noise
		  //the max lenght is 750 ns if there are >10 PMTs, 2000 ns if less
		  //if there are too many channels above threshold in a single window, 
		  // it is likely that 
		  //the full pulse is above threshold because of baseline fluctuations
		  int ichanMaxLen = (npmt > 10) ? 750 : 2000;	
		  if (ichanint > ichanMaxLen)
		    {
		      if (the_event.isnoise[ipmt] == 0)
			the_event.isnoise[ipmt] = 1;
		    }

#ifdef ML_FOR_PHOTONS 
		  if (ichanintwindow)
		    {
		      float correctionwindow = 0.5*baseright*ichanintwindow;
		      s1inwin[ipmt] += (integralwindow - correctionwindow);
		    }
#endif
		  i = jj-windowLenght-2; //update i at the end of the current block
		  lastBlock = i+1;
		}
	    }
	  if (p->is_ser_found)
	    {
	      s1tot /=  p->PMTSer[ipmt];
	      s1fast /= p->PMTSer[ipmt];
#ifdef ML_FOR_PHOTONS 
	      s1inwin[ipmt] /= p->PMTSer[ipmt];
#endif
	    }

	  if (firstBlock)
	    {
	      the_event.tp[ipmt] = firstBlock;
	      the_event.sp[ipmt] = s1tot;
	      the_event.spf[ipmt] = s1fast;
	    }
	  else
	    {
	      the_event.tp[ipmt] = 0;
	      the_event.sp[ipmt] = 0.;
	      the_event.spf[ipmt] = 0.0;
	    }
	  cout << "Primary-INTE: PMT#" << ipmt+1 << " " << the_event.tp[ipmt] << " " << 
	    the_event.sp[ipmt] << " " << 
	    the_event.spf[ipmt] << " ";
	  if (the_event.isnoise[ipmt]) 
	    cout << "NOISE "; 
	  cout << endl;
	  //if(p->isPMTOn[ipmt]) the_event.s1intetime = (lastBlock-firstBlock)*samplingfreq;
	}
    }
  //  myfile.close();
  
#ifdef ML_FOR_PHOTONS
  //Sort
  //  sort(singlePheTimeList.begin(),singlePheTimeList.end());
  //for (size_t i=0;i<singlePheTimeList.size();i++)
  // cout << "PHOTON: " << i << " " << singlePheTimeList[i] << endl;
  //
  float ave = 0.0;
  the_event.numberofpheinwindow = singlePheTimeList.size();
  if (the_event.numberofpheinwindow > 5)
    {
      for (size_t i=0;i<singlePheTimeList.size();i++)
	ave += singlePheTimeList[i];
      ave /= singlePheTimeList.size();
      the_event.averagephetime = ave;
    }
  float S1inwindow = 0;
  for (int ipmt=0;ipmt<npmt;ipmt++)
    {
      if (p->isPMTOn[ipmt]) //take only active PMTs
	S1inwindow += s1inwin[ipmt];
    }
  the_event.s1inwindow = S1inwindow;

#endif
}
