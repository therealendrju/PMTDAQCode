/***************************************************************************
 *   Copyright (C) 2009 by andrzej szelc   *
 *   szelc@lngs.infn.it   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "process.h"
#include <TF1.h>
#include <iostream>
#include <TMath.h>
#include <sys/stat.h>
//#include "wep.h"
#include "../mcrbee10/wep.h"
#include "../mcrbee10/processor_code.h"

extern  int process::NEXP;
extern  int process::NGAUSS;


wep *ppp=new wep();
TStyle *gStyle;
double fitfunc(double *x, double *par);




process::process(int nchans):NCHANNELS(nchans)
{
TH1::AddDirectory(kFALSE); 
ser_chan=new channel*[NCHANNELS]();
integ_chan=new channel*[NCHANNELS]();
raw_chan=new channel*[NCHANNELS]();
sum_chan=new channel*[3]();

sum_chan[0]=NULL;
sum_chan[1]=NULL;
sum_chan[2]=NULL;
  
for(int i=0;i<NCHANNELS;i++)
	{ser_chan[i]=NULL;
	integ_chan[i]=NULL;
	raw_chan[i]=NULL;
	fHist[i]=NULL;
	fIntHist[i]=NULL;
	fRawHist[i]=NULL;
	
	}
plow=NULL;
phigh=NULL;
pval=NULL;

fsumHist[0]=NULL;
fsumHist[1]=NULL;
fsumHist[2]=NULL;

NEXP=2;
NGAUSS=2;

//inin=NULL;

gStyle=new TStyle();
gStyle->SetOptFit();
}


process::~process()
{
delete gStyle;
for(int i=0;i<NCHANNELS;i++)
	{if(ser_chan[i]!=NULL)
		delete ser_chan[i];
	if(integ_chan[i]!=NULL)
		delete integ_chan[i];
	if(raw_chan[i]!=NULL)
		delete raw_chan[i];
	}	
	
	if(sum_chan[0]!=NULL)
		delete sum_chan[0];
	if(sum_chan[1]!=NULL)
		delete sum_chan[1];
	if(sum_chan[2]!=NULL)
		delete sum_chan[2];
	
	delete [] sum_chan;	
	
if(plow!=NULL)
	delete plow;
if(phigh!=NULL)
	delete phigh;
if(pval!=NULL)
	delete pval;

}


int process::read_channel(int n)
{

TH1F * read_hist=new TH1F("chan","chan",100,1,100);
//////reading into a histogram

ser_chan[n]=new channel(read_hist);

return 0;
}


int process::read_channels(int run,int event)
{



  char fname[20];
  sprintf(fname,"run%d.root",run);

  //if(inin!=NULL)
    //inin->Close();

//  struct stat stFileInfo;
 // int intStat;

  // Attempt to get the file attributes
//   intStat = stat(fname,&stFileInfo);
//   if(intStat != 0) { 
    cout << "no file found, creating new" << endl;
    Fill_events(run,event,fname);
 // }
//   else
//     {
//         
//       inin=TFile::Open(fname);
//       cout << "file opened " << inin->IsOpen() <<endl;
//       for(int np=0;np<NCHANNELS;np++)
// 	{
// 	 ser_chan[np]=new channel((TH1F *)inin->Get(Form("ser_%d",np)));
// 	}
//     }


/*
for(int i=0;i<NCHANNELS;i++)
{
=new TH1F(Form("chan %d",i),Form("chan %d",i),100,1,100);
//////reading into a histogram
ser_chan[i]=new channel(read_hist);
}*/

return 0;

}




// int process::fit_channel(int n)
// {
// 
// double converged=1.4;
// 
//   const Int_t NPARS=1+NEXP*2+2+NGAUSS;
//   TString funcname=Form("func nr%d",n);
// 
// int St_fit=get_fitting_start(get_channel(n)->get_histo());
// 
// TF1 * func=new TF1(funcname,fitfunc,St_fit,800,NPARS);
// set_function_pars(func,pval,plow,phigh);
// 
// //loop on fits, if chisquare is good, return 1, if fit did not converge return 0;
// int iter=0;
// double ratio;
// 	do
// 	{
//        	get_channel(n)->get_histo()->Fit(funcname,"BRM");
// 	ratio=func->GetChisquare()/func->GetNumberFitPoints();
//         std::cout << "  ratio  " << ratio<< " iter" << iter << std::endl;
// 
// 	iter ++;
// 	}while((ratio > converged) && (iter <8));
// 
// save_results_in_channel(func,n);
// 
// 
// delete func;
// 
//   if((iter>=8) || (ratio>=converged)) 
// 	{
// 	return 0;
// //ad_fit_flag=1;
// 	}
//   else{
// 	return 1;
// 	}	
// 
// }



// void process::get_params_from_file()
// {
//   Int_t temp;
//   
//   const Int_t NPARS=1+NEXP*2+2+NGAUSS;
// 
// if(plow!=NULL)
// 	delete plow;
// if(phigh!=NULL)
// 	delete phigh;
// if(pval!=NULL)
// 	delete pval;
// 
// 
// plow =new double[NPARS];
// phigh =new double[NPARS];
// pval =new double[NPARS];
// 
// 
//   ifstream inp("params.txt"); 
//   if(inp.is_open())
//     {inp>>temp;inp>>temp;
//     for(Int_t j=0;j<NPARS;j++)
//       {inp >> pval[j] >> plow[j] >> phigh[j];
//      std::cout << pval[j] <<" " << plow[j]<<" "<< phigh[j] << std::endl;}
//     inp.close();
//     }
//   else
//     {
//       pval[0]=1;     plow[0]=0;  phigh[0]=30;
//       pval[1]=20;    plow[1]=0;  phigh[1]=100;
//       pval[2]=-0.002;plow[2]=-1.;phigh[2]=0.0;
//       pval[3]=15;    plow[3]=0;  phigh[3]=1000;
//       pval[4]=-0.001;plow[4]=-1.;phigh[4]=0;
//       pval[5]=120;   plow[5]=50;phigh[5]=250;
//       pval[6]=20;   plow[6]=10; phigh[6]=200;
//       pval[7]=20;   plow[7]=5; phigh[7]=5000;
//       pval[8]=1;     plow[8]=0;  phigh[8]=500;
//     
//     }
//  // for(Int_t i=9;i<15;i++)
//    // {plow[i]=0; pval[i]=0; phigh[i]=0;}
// 
// }
// 
// 
// void process::set_NGAUSS(int ng)
// {
// NGAUSS=ng;
// }


/*
void process::get_param_arrays(double *pv,double *pl,double *ph)
{

  const Int_t NPARS=1+NEXP*2+2+NGAUSS;

// pv=pval;
// pl=plow;
// ph=phigh;
for(int i=0;i<NPARS;i++)
	{
	std::cout << "----   " << pval[i] << std::endl;
	pv[i]=pval[i];
	pl[i]=plow[i];
	ph[i]=phigh[i];
	}
}


void process::set_param_arrays(double *pv,double *pl,double *ph)
{
const Int_t NPARS=1+NEXP*2+2+NGAUSS;

for(int i=0;i<NPARS;i++)
	{
	std::cout << "----   " << pval[i] << std::endl;
	pval[i]=pv[i];
	plow[i]=pl[i];
	phigh[i]=ph[i];
	}




}*/



channel * process::get_channel(int n)
{
return ser_chan[n];

}


channel * process::get_intchannel(int n)
{
	return integ_chan[n];

}

channel * process::get_sum_channel(int n)
{
return sum_chan[n];

}


channel * process::get_raw_channel(int n)
{
	return raw_chan[n];

}


// void process::set_function_pars(TF1 * func,Double_t pval[],Double_t plow[],Double_t phigh[])
// {
//   const Int_t NPARS=1+NEXP*2+2+NGAUSS;
//   
//   for(Int_t j=0;j<NPARS;j++)
//     {
//       func->SetParameter(j,pval[j]);
//       func->SetParLimits(j,plow[j],phigh[j]);
//       
//       cout << "Setting parameter j" << pval[j] << " "<< plow[j]<< " "<< phigh[j] <<endl;
//       
//     }
//       
//   for(Int_t j=1;j<=NEXP;j++)
//     {
//       char * name1=new char[5]; 
//       char * name2=new char[5];
//       sprintf(name1,"B%d",j);
//       sprintf(name2,"T%d",j);
//       func->SetParName(2*j-1,name1);
//       func->SetParName(2*j,name2);  
//     }
//   
//   func->SetParName(2*NEXP+1,"Xo");
//   func->SetParName(2*NEXP+2,"Sigma");
// }



// void process::save_results_in_channel(TF1 * func,int channel)
// {
// results rests;
// 
// int ParXo=NEXP*2+1;    // number of Xo param.
// int ParSig=NEXP*2+2;   // number of sigma param
// int ParN=NEXP*2+3;
//   /*--------------results input -------------*/
// 
// rests.xo= func->GetParameter(ParXo);
// rests.xo_err=func->GetParError(ParXo);
// rests.sigma=func->GetParameter(ParSig);
// rests.sigma_err=func->GetParError(ParSig);
// rests.N=func->GetParameter(ParN);
// rests.N_err=func->GetParError(ParN);
// 
// rests.chisq=func->GetChisquare();
// 
// 
// ser_chan[channel]->write_results(rests);
// }




// int process::get_fitting_start(TH1F * myhist)
// {
//   int St_fit=0;
//   for(Int_t i=0;i<8192;i++)
//     if( myhist->GetBinContent(i) >=5  )
//       {
// 	St_fit=i+5;      //arbitrary number
// 	break;
//       }
// //  St_fit*=(Int_t)Divis;
//  return  St_fit+=2;
//  
// }



void process::Fill_events(int run,int event,char * fname)
  //currently only for first ten thousand evts/
{
// 	for(int np=0;np<NCHANNELS;np++)
// 	{
// 		if(ser_chan[np]!=NULL)
// 			{delete ser_chan[np];
// 			ser_chan[np]=NULL;
// 			}
// 		
// 		if(integ_chan[np]!=NULL)
// 			{delete integ_chan[np];
// 			integ_chan=NULL;
// 			}
// 			
// 		if(raw_chan[np]!=NULL)
// 			{delete raw_chan[np];
// 			raw_chan=NULL;
// 			}	
// 	}
	
	cout << " In Fill Events " << endl;
	
  //standard before settings
  int initial=0;
  int iargc=0;
  int limit=5000;
  char path[20];
  //sprintf(path,"/media/LaCie/Run_2008/Run%06d/",run);
  //sprintf(path,"/mnt/disk2/Run%06d/",run);
  sprintf(path,"/data/Run%06d/",run);
  
//   if(after)
//     {
//       initial=get_last_file_number(path)-limit;
//       if(initial<0)
// 	{
// 	  limit=get_last_file_number(path);
// 	  initial=0;
// 	  iargc=0;		
// 	}
//       else
// 	{
 	  iargc=initial;
// 
// 	}
// 
//     }

  cout << "initial, iargc, limit" << initial << " " << iargc << " " <<limit << endl;

  char argv[100];



    
    cout << "++++ fname " << fname << endl;
  //init(fname);

  ppp->clear_functions();
    ppp->isGUI=true;
    ppp->isSer=false;
    ppp->debug = false;
  ppp->set_start(start_ev);

  ppp->add_function(get_baseline);
  ppp->add_function(process_pulse);
  //ppp->add_function(fit_s1);
  ppp->add_function(fill_values);
  //ppp->add_function(fit_s1);
  // //  ppp->add_function(export_ev);
  // //  ppp->add_function(eval_s1_nofield);
  // //  ppp->add_function(fill_values);
  //ppp->set_end(end_ev);


  // ifile = 2;
  //iargc = argc-2;
  int ret=0;

    sprintf(argv,"%sRun%06d_%06d",path,run,iargc);	
    cout << "--------opening "  <<argv << endl;		
    ppp->open_file(argv);
  
   if( (run != 0) && (event != 0) ) {
	  cout << "+++++run = " << run << " event = " << event << endl;
	  ret=ppp->get_event(run, event);
	//ret=ppp->readevent(event);
	} else {
	  ret=ppp->readevent();
	}
    
	//cout << "------- ret" << ret <<endl;
	
    if(ret == 0) 
    {
	ppp->process_event();
	//cout << "++++++++ processed event " <<endl;
    }
    
    
    
    //do {
    //  ret = ppp->readevent();
      
      cout << argv << endl;
   // } while (ret >= 0);
    ppp->close_file();
    iargc+=1000;	 
    cout << iargc << " " << iargc-initial << endl;
  
  float * samples=new float[ppp->get_nsamples()];
  
  for(int i=0;i< ppp->get_nsamples();i++)
  	samples[i]=i;
  
  
if(fsumHist[0]==NULL)
    fsumHist[0] =new TH1F(Form(" Sum, run %d, evt, %d ",run,event),Form(" Sum, run %d, evt, %d ",run,event), ppp->get_nsamples(), 0., ppp->get_nsamples());
else
    {
	fsumHist[0]->Reset();
	fsumHist[0]->SetTitle(Form(" Sum, run %d, evt, %d ",run,event));
     }

if(fsumHist[1]==NULL)
    fsumHist[1] = new TH1F(Form(" Integral, run %d, evt, %d ",run,event),Form(" Integral, run %d, evt, %d ",run,event), ppp->get_nsamples(), 0., ppp->get_nsamples());
else
    {
	fsumHist[0]->Reset();	
	fsumHist[1]->SetTitle(Form(" Integral, run %d, evt, %d ",run,event));
     
	}

if(fsumHist[2]==NULL)
    fsumHist[2] = new TH1F(Form(" Integral corrected, run %d, evt, %d ",run,event),Form(" Integral corrected, run %d, evt, %d ",run,event), ppp->get_nsamples(), 0., ppp->get_nsamples());
else
     {
	fsumHist[2]->Reset();
	fsumHist[2]->SetTitle(Form(" Integral corrected, run %d, evt, %d ",run,event));

     }	
    //fHistIntegWin[ipmt]->
    
// for(int i=0; i<ppp->get_nsamples(); i++) {
//	fsumHist[0]->SetBinContent(i,0);
//	fsumHist[1]->SetBinContent(i,0);
//	fsumHist[2]->SetBinContent(i,0);
//	}




 		



  for(int np=0;np<NCHANNELS;np++)
    {
    int iii=ppp->get_nsamples()*np;
      cout << "filling ev nr " << np << " " << ppp->get_nsamples() << endl; 
	
	if(fHist[np]==NULL)
      		{fHist[np] = new TGraph(ppp->get_nsamples());}
	if(fIntHist[np]==NULL)
      		fIntHist[np] = new TGraph(ppp->get_nsamples());
	if(fRawHist[np]==NULL)
      		fRawHist[np] = new TGraph(ppp->get_nsamples());	
		
		
		 //TH1F(Form(" channel int %d ",np),Form(" channel int %d ",np), ppp->get_nsamples(), 0., ppp->get_nsamples());
	//else
	//	fIntHist[np]->Scale(0.);
//TH1F(Form(" channel raw %d ",np),Form(" channel raw %d ",np), ppp->get_nsamples(), 0., ppp->get_nsamples());      
	//else
		//fRawHist[np]->Scale(0.);
	       

	for(int i=0; i<ppp->get_nsamples(); i++) 
       		{
       
       
       
//	       fHist[np]->Fill(i+1,adcc[i+iii]);
    		fHist[np]->SetPoint(i,i,adcc[i+iii]);
//	       (ppp->get_nsamples(),samples,&adcc[iii]);
	//(Form(" channel %d ",np),Form(" channel %d ",np), ppp->get_nsamples(), 0., ppp->get_nsamples());
	//else
		//fHist[np]->Scale(0.);
	       
	       
	       float val = 0.0;
	       if(isAboveThreshold[i+iii] == true) val = pmtthreshold[np];
	       fIntHist[np]->SetPoint(i,i, val);
	       fIntHist[np]->SetLineColor(kRed);
	       val=0.0;
	       
       	//	if(np==2)
	//	  cout << i << " " <<	adcc[i+iii] <<" "<< ppp->data[i+iii] << endl;
	       
	       
	       fRawHist[np]->SetPoint(i,i,ppp->data[i+iii]);
	       
	       fsumHist[0]->Fill(i+1,adcc[i+iii]);
 
	       
	       //int val=0.0;
             
	     //  if(isAboveThreshold[i+iii] == true) val = (float) pmtthreshold[ipmt];
		//       fHistIntegWin[ipmt]->SetBinContent(i+1, val);
	
	         
     //  if(adcc[i+iii] && (i>10 && i <20 ))
       //cout << " ::: chan, adcc " <<np << " " << adcc[i+iii] << endl;
		if(ser_chan[np]!=NULL)
	       		ser_chan[np]->set_histo(fHist[np]);
		else
			ser_chan[np]=new channel(fHist[np]);

		if(integ_chan[np]!=NULL)
	       		integ_chan[np]->set_histo(fIntHist[np]);
		else
			integ_chan[np]=new channel(fIntHist[np]);

		if(raw_chan[np]!=NULL)
	       		raw_chan[np]->set_histo(fRawHist[np]);
		else
			raw_chan[np]=new channel(fRawHist[np]);
       		}
	//	cout << "+++++++++ float val " << pmtthreshold[np] <<	endl;
	//	cout << " ----Threshold of pmt " << np << " " << pmtthreshold[np] << endl;

       }
       double integ=0,integcorr=0;
   for(int i=0; i<ppp->get_nsamples(); i++) 
   	{
       for(int np=0;np<NCHANNELS;np++)
       		{     
		int iii=ppp->get_nsamples()*np;
		integ+=	adcc[i+iii];
		integcorr+=adccsumcorr[i+iii];
		}
	fsumHist[1]->Fill(i+1,integ);
	fsumHist[2]->Fill(i+1,integcorr);		
 	}
       
 
     
    
	//for(int i=0; i<ppp->get_nsamples(); i++) {
		//if( (i>10 && i <20 ))
	  //     cout << " .... chan, histo1, histo2 " <<np << " " << fHist[np]->GetBinContent(i+1) <<" " << ser_chan[np]->get_histo()->GetBinContent(i+1) <<  endl;
	//}
     if(sum_chan[0]!=NULL)
     	sum_chan[0]->set_histo(fsumHist[0]);
     else
	sum_chan[0]=new channel(fsumHist[0]);

     if(sum_chan[1]!=NULL) 		
     sum_chan[1]->set_histo(fsumHist[1]);
     else	
	sum_chan[1]=new channel(fsumHist[1]);

     if(sum_chan[2]!=NULL) 		
     sum_chan[2]->set_histo(fsumHist[2]);
     else	
	sum_chan[2]=new channel(fsumHist[2]);


     sum_chan[2]->set_histo(fsumHist[2]);
 // term();
delete []samples;
}


void process::write_ser_windows(int channel)
{

// for(int i=0;i<ser_peaks[channel].size();i++)
// 	cout << "window nr " << i << " " << ser_peaks[channel][i].tstart << " " << ser_peaks[channel][i].tstop << " " << ser_peaks[channel][i].integ << " " << ser_peaks[channel][i].amp << " " << ser_peaks[channel][i].length << " " << ser_peaks[channel][i].npmt << endl;

	
	
	for(int i=0;i<ser_peaks[channel].size();i++)
	{
		
		float max=0,thresh=0;
		int index=ser_peaks[channel][i].npmt*ppp->nSAMPLES;
		for(int j=ser_peaks[channel][i].tstart;j<ser_peaks[channel][i].tstop;j++)
		{
			if(adcc[j+index]>max)
				{max=adcc[j+index];
				thresh=pmtthreshold[ser_peaks[channel][i].npmt];
				}
			}
		
			
		printf("window nr %d  %d  %d %f %f %d %d %f %f \n",i,ser_peaks[channel][i].tstart,ser_peaks[channel][i].tstop, ser_peaks[channel][i].integ , ser_peaks[channel][i].amp ,ser_peaks[channel][i].length , ser_peaks[channel][i].npmt, max,thresh);
	}
	printf(" \n \n \n");
		
	
}






/////////////////////////// end of class, but inside here needed to fit.





double fitfunc(double *x, double *par)
{
 //  const Int_t ng=NGAUSS;
  Double_t arg[10]={0};                 //temporary
  // Int_t NPARS=3+2+NGAUSS;
  
  Double_t start=par[0];
  Double_t iter=0;
  for(Int_t j=0;j<process::NEXP*2;j+=2)
    {
      iter+=TMath::Exp(par[j+1]+par[j+2]*x[0]);
    }

  for(Int_t j=0;j<process::NGAUSS;j++)
    {
      if (par[process::NEXP*2+2])
	{  
	  arg[j] = (x[0]- (j+1)*par[process::NEXP*2+1])/(TMath::Sqrt((double)j+1)*par[process::NEXP*2+2]);   // gaussian argument
	  
	}
      iter+=par[j+process::NEXP*2+3]*TMath::Exp(-0.5*arg[j]*arg[j]);
      

    }
  Double_t fitval=start+iter;
  return fitval;
  
}




// // serves to open code with a single file as parameter.

// void process::Fill_events(Event ** event,const char * fname)
//   //currently only for first ten thousand evts/
// {
// 
// 
// 
// 
// 
//   if(inin!=NULL)
//     inin->Close();
// 
//   struct stat stFileInfo;
//   int intStat;
// 
//   // Attempt to get the file attributes
//   intStat = stat(fname,&stFileInfo);
//   if(intStat != 0) { 
//     cout << "no file found, exiting" << endl;
//     std::exit(-1);
//   }
//   else
//     {
//         
//       inin=TFile::Open(fname);
//       cout << "file opened " << inin->IsOpen() <<endl;
//       for(int np=0;np<NFILES;np++)
// 	{
// 	  event[np]->histo=(TH1F *)inin->Get(Form("ser_%d",np));
// 	}
//     }
// 
// }




















