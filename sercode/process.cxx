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

//#include "wep.h"
#include "wep.h"
#include "processor_code.h"
#include <iomanip>
#include <TVector.h>
#include <TParameter.h>

extern  int process::NEXP;
extern  int process::NGAUSS;


wep ppp;
TStyle *gStyle;
double fitfunc(double *x, double *par);




process::process(int nchans):NCHANNELS(nchans)
{

ser_chan=new channel*[NCHANNELS]();
amp_chan=new channel*[NCHANNELS]();
leng_chan=new channel*[NCHANNELS]();
  
for(int i=0;i<NCHANNELS;i++)
	{ser_chan[i]=NULL;
	amp_chan[i]=NULL;
	leng_chan[i]=NULL;
	}
plow=NULL;
phigh=NULL;
pval=NULL;

NEXP=1;
NGAUSS=4;

inin=NULL;
char DBname[20];
#ifdef	__MY_SQL_TYPE__
	printf("__MY_SQL_TYPE__ defined \n");
	sprintf(DBname, "warp100");
#elif __MY_SQL_MC__
	printf("__MY_SQL_MC__ defined \n");
	sprintf(DBname, "warp_mc");
#else
	printf("__MY_SQL_TYPE__ not defined \n");
	sprintf(DBname, "warp_other");
#endif
  //sql_interface DBinput("warp100");
  cout << " using database " << DBname  << endl;;

//std::exit(-1);
  //sql=new sql_interface(DBname);

gStyle=new TStyle();
gStyle->SetOptFit();
}


process::~process()
{

for(int i=0;i<NCHANNELS;i++)
	{if(ser_chan[i]!=NULL)
		delete ser_chan[i];

	if(amp_chan[i]!=NULL)
		delete amp_chan[i];

	if(leng_chan[i]!=NULL)
		delete leng_chan[i];
	
		
		
		}
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


//int process::read_channels(int run)
int process::read_channels(int run,vector<string> filelist)
{



  // Attempt to get the file attributes
  //int intStat = stat(fname,&stFileInfo);
  if(strstr(filelist[0].c_str(),"root")==NULL) { 
    cout << "no file found, creating new" << endl;
    char fname[20];
     sprintf(fname,"run%d.root",run);
    Fill_events(run,fname,filelist);
  }
  else
    {
       if(inin!=NULL)
    inin->Close();
      
      inin=TFile::Open(filelist[0].c_str());
      cout << "file opened " << inin->IsOpen() <<endl;
      TParameter<float> *threshold=NULL;
      TParameter<float> *chan_before=NULL;
	TParameter<float> *chan_after=NULL;
	TParameter<float> *integr_window=NULL;

      threshold=(TParameter<float> *)inin->Get("threshold");
      chan_before=(TParameter<float> *)inin->Get("chan_before");
      chan_after=(TParameter<float> *)inin->Get("chan_after");
	integr_window=(TParameter<float> *)inin->Get("integr_window");


      
    //  TVectorF *paramin=(TVectorF *)inin->Get("params");
    if(threshold!=NULL && chan_before !=NULL &&  chan_after !=NULL &&  integr_window !=NULL )
       {
      cout << "++++ opening " << threshold->GetVal() << " " << chan_before->GetVal() << " " <<  chan_after->GetVal() << " " << integr_window->GetVal() << " " <<
		endl;
		
	set_ser_params(threshold->GetVal(),chan_before->GetVal(),chan_after->GetVal(),integr_window->GetVal());	
	
	}
     else
     	{	
	set_ser_params(0.0,0,0,0);	
	}
	
		
      for(int np=0;np<NCHANNELS;np++)
	{
	 ser_chan[np]=new channel((TH1F *)inin->Get(Form("ser_%d",np)));
	 amp_chan[np]=new channel((TH1F *)inin->Get(Form("amp_%d",np)));
	 leng_chan[np]=new channel((TH1F *)inin->Get(Form("length_%d",np)));
	}
    }


/*
for(int i=0;i<NCHANNELS;i++)
{
=new TH1F(Form("chan %d",i),Form("chan %d",i),100,1,100);
//////reading into a histogram
ser_chan[i]=new channel(read_hist);
}*/

return 0;

}




int process::fit_channel(int n)
{

double converged=1.4;

  const Int_t NPARS=1+NEXP*2+2+NGAUSS;
  TString funcname=Form("func nr%d",n);

int St_fit=get_fitting_start(get_channel(n)->get_histo());

TF1 * func=new TF1(funcname,fitfunc,St_fit,800,NPARS);
func->SetNpx(500);
set_function_pars(func,pval,plow,phigh);

//loop on fits, if chisquare is good, return 1, if fit did not converge return 0;
int iter=0;
double ratio;
	do
	{
       	get_channel(n)->get_histo()->Fit(funcname,"BRM");
	ratio=func->GetChisquare()/func->GetNumberFitPoints();
        std::cout << "  ratio  " << ratio<< " iter" << iter << std::endl;

	iter ++;
	}while((ratio > converged) && (iter <8));

save_results_in_channel(func,n);

//cout << " +++++ " << func->GetParameter(1+2*NEXP+2)<<" " << func->GetParameter(1+2*NEXP+3)  << endl;

bool test=(func->GetParameter(1+2*NEXP+2) < func->GetParameter(1+2*NEXP+3));

delete func;

  if((iter>=8) || (ratio>=converged) || test ) 
	{
	cout << "breaking with 0 " << endl;	
	return 0;
//ad_fit_flag=1;
	}
  else{
	return 1;
	}	

}



void process::get_params_from_file()
{
  Int_t temp;
  
  const Int_t NPARS=1+NEXP*2+2+NGAUSS;

if(plow!=NULL)
	delete plow;
if(phigh!=NULL)
	delete phigh;
if(pval!=NULL)
	delete pval;


plow =new double[NPARS+5];
phigh =new double[NPARS+5];
pval =new double[NPARS+5];


  ifstream inp("params.txt"); 
  if(inp.is_open())
    {inp>>temp;inp>>temp;
    for(Int_t j=0;j<NPARS;j++)
      {inp >> pval[j] >> plow[j] >> phigh[j];
     std::cout << pval[j] <<" " << plow[j]<<" "<< phigh[j] << std::endl;}
    inp.close();
    for(Int_t j=NPARS;j<NPARS+5;j++)
	    pval[j] = plow[j] = phigh[j] =0;
    }
  else
    {
      pval[0]=1;     plow[0]=0;  phigh[0]=30;
      pval[1]=20;    plow[1]=0;  phigh[1]=100;
      pval[2]=-0.002;plow[2]=-1.;phigh[2]=0.0;
      pval[3]=15;    plow[3]=0;  phigh[3]=1000;
      pval[4]=-0.001;plow[4]=-1.;phigh[4]=0;
      pval[5]=120;   plow[5]=50;phigh[5]=250;
      pval[6]=20;   plow[6]=10; phigh[6]=200;
      pval[7]=20;   plow[7]=5; phigh[7]=5000;
      pval[8]=1;     plow[8]=0;  phigh[8]=500;
    
    }
 // for(Int_t i=9;i<15;i++)
   // {plow[i]=0; pval[i]=0; phigh[i]=0;}

}


void process::set_NGAUSS(int ng)
{
NGAUSS=ng;
}



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




}



channel * process::get_channel(int n)
{
return ser_chan[n];

}

channel * process::get_ampchannel(int n)
{
	return amp_chan[n];

}

channel * process::get_lengthchannel(int n)
{
	return leng_chan[n];

}


void process::set_function_pars(TF1 * func,Double_t pval[],Double_t plow[],Double_t phigh[])
{
  const Int_t NPARS=1+NEXP*2+2+NGAUSS;
  
  for(Int_t j=0;j<NPARS;j++)
    {
      func->SetParameter(j,pval[j]);
      func->SetParLimits(j,plow[j],phigh[j]);
      
      cout << "Setting parameter j" << pval[j] << " "<< plow[j]<< " "<< phigh[j] <<endl;
      
    }
      
  for(Int_t j=1;j<=NEXP;j++)
    {
      char * name1=new char[5]; 
      char * name2=new char[5];
      sprintf(name1,"B%d",j);
      sprintf(name2,"T%d",j);
      func->SetParName(2*j-1,name1);
      func->SetParName(2*j,name2);  
    }
  
  func->SetParName(2*NEXP+1,"Xo");
  func->SetParName(2*NEXP+2,"Sigma");
}



void process::save_results_in_channel(TF1 * func,int channel)
{
results rests;

int ParXo=NEXP*2+1;    // number of Xo param.
int ParSig=NEXP*2+2;   // number of sigma param
int ParN=NEXP*2+3;
int ParExp=1;
  /*--------------results input -------------*/

rests.xo= func->GetParameter(ParXo);
rests.xo_err=func->GetParError(ParXo);
rests.sigma=func->GetParameter(ParSig);
rests.sigma_err=func->GetParError(ParSig);
rests.N=func->GetParameter(ParN);
rests.N_err=func->GetParError(ParN);
rests.exp_a=func->GetParameter(ParExp);
rests.exp_t=func->GetParameter(ParExp+1);

//---------------- search for the valley ----------//
double min=100000000;
double val=0;

int St_fit=get_fitting_start(get_channel(channel)->get_histo());
 TString funcname=Form("func nr%d",channel);


for(double nn=St_fit;nn<=rests.xo;nn+=0.1)
	{
	if(min > func->Eval(nn))
		{val=nn;
		 min=func->Eval(nn);
		}
	}

if((val >= rests.xo-3) || val <= St_fit)
      {
	cout << "result did not find valley" << endl;
	  TF1 * g1=new TF1("g1","gaus",5,8000);
  TF1 * e1=new TF1("e1","expo",5,8000);
  
 // TF1 * tt=new TF1("tt","expo",5,8000);
  
 
      Double_t p0=func->GetParameter(1);
      e1->SetParameters(p0,func->GetParameter(2));

   //Gauss loop
       
g1->SetParameters(func->GetParameter(2+3),(1)*rests.xo,TMath::Sqrt((double)(1))*rests.sigma);
  
  
	  for(double nn=St_fit;nn<=rests.xo;nn+=0.01)
		{
		if(e1->Eval(nn)-g1->Eval(nn)<0)
			{val=nn;
			 break;
			}
		}

      
//      val = rests.xo - rests.sigma*1.5;

    delete e1;
    delete g1;

	}
rests.valley=val;
//   
rests.val_of_valley=func->Eval(val);
int max=0;
for(double nn=rests.xo-rests.sigma;nn<=rests.xo+rests.sigma;nn+=0.1)
	{
	if(max < func->Eval(nn))
		{val=nn;
		 max=func->Eval(nn);
		}
	}


rests.val_of_peak=max;


//----------------end search -----------------------//

if(func->GetNDF())
	rests.chisq=func->GetChisquare()/func->GetNDF();


ser_chan[channel]->write_results(rests);
}




int process::get_fitting_start(TH1F * myhist)
{
  int St_fit=0;
  for(Int_t i=0;i<8192;i++)
    if( myhist->GetBinContent(i) >=5  )
      {
	St_fit=i+3;      //arbitrary number
	break;
      }
//  St_fit*=(Int_t)Divis;
 return  St_fit+=2;
 
}


void process::set_ser_params(float thresh,int intbef,int intaft,int window)
{
std::map<string,double> params;
params["threshold"]=thresh;
params["intbef"]=intbef;
params["intaft"]=intaft;
params["window"]=window;
  
set_ser_params(params);
  
}


void process::set_ser_params(std::map<string,double> params)
{
	
	
	if((int)(params["threshold"]+0.5))
		ppp.threshold=params["threshold"];
	
	if((int)params["intbef"])	
		ppp.chan_before=(int)params["intbef"];
	
	
	if((int)params["intaft"])	
		ppp.chan_after=(int)params["intaft"];
	
	if((int)params["window"])	
		ppp.chan_before=(int)params["window"];
	
	if((int)params["onset"])	
		ppp.chan_before=(int)params["onset"];
	
	if((int)params["duration"])	
		ppp.chan_before=(int)params["duration"];
	
	
	
}	

void process::Fill_events(int run,char * fname,vector<string> filelist)
  //currently only for first ten thousand evts/
{

 
 

  for(int np=0;np<NCHANNELS;np++)
    {
      if(ser_hist[np]!=NULL)
	delete ser_hist[np];

	if(ser_leng[np]!=NULL)
	delete ser_leng[np];
	
	if(ser_amp[np]!=NULL)
	delete ser_amp[np];
    }
    ppp.isSer=true;
    
    ppp.nPMT=NCHANNELS;
  init(fname,&ppp);

  ppp.clear_functions();
  	
  ppp.set_start(start_ev);
 // ppp.add_function(eval_baseline);
  // //  ppp.add_function(make_fft);
  // //  ppp.add_function(make_backfft);
 // ppp.add_function(median_filter);
//  ppp.add_function(eval_ser);

  ppp.add_function(get_baseline);
  ppp.add_function(process_pulse);

  // //  ppp.add_function(export_ev);
  // //  ppp.add_function(eval_s1_nofield);
  // //  ppp.add_function(fill_values);
  ppp.set_end(end_ev);


  // ifile = 2;
  //iargc = argc-2;
  int ret=0;

  std::cout << "---- files list: size" << filelist.size() << " " <<filelist[0] <<  std::endl;
  for(unsigned int nfile=0;nfile<filelist.size();nfile++)
{

    ppp.open_file(filelist[nfile].c_str());
    do {
      ret = ppp.readevent();
      if(ret == 0) 
	{ppp.process_event();
	//	start_ev();
	//	eval_baseline(&ppp);
	//	median_filter(&ppp);
	//	eval_ser(&ppp);
	//	end_ev();
	}
     // cout << argv << endl;
    } while (ret >= 0);
    ppp.close_file();
    
    
  } 




  for(int np=0;np<NCHANNELS;np++)
    {
      cout << "filling ev nr " << np <<endl; 
      ser_hist[np]->Rebin(4); 	
     ser_chan[np]=new channel(ser_hist[np]);
     amp_chan[np]=new channel(ser_amp[np]);
     leng_chan[np]=new channel(ser_leng[np]);

    }

  term(&ppp);

}



void process::save_results_toDB(int runin,int * onoff)
{
results sers[NCHANNELS+1];
  for(int i=1;i<=NCHANNELS;i++)
    sers[i]=ser_chan[i-1]->get_results();

    sql->insert_pmt_sers(sers,onoff,NCHANNELS,runin,0,ppp.vSCALE);
    sql->insert_pmt_sers(sers,onoff,NCHANNELS,runin,1,ppp.vSCALE);

//    if(intStat!=0)  //data were reanalyzed and not extracted from the root file.
 //   {
	    sql->insert_ser_parameters(runin,ppp.threshold,ppp.integr_window,ppp.chan_before,ppp.chan_after);
 //   }

}


ostream& operator<<(ostream& output,results a)
{
	
	output << setw(7) << a.xo << " " <<setw(7)<< a.sigma <<  " " <<setw(7) << a.N << " " << setw(7) << a.chisq <<" "<<setw(7) << a.val_of_peak<<" " << setw(7) << a.val_of_valley;
	output << setw(7) << a.valley;
	
	return output;	
}



void process::save_results_to_file(int runin)
{


// save SER x0, sigma and valley to txt file for mcrbee to read in 
	ofstream outfile (Form("ser_run%d.txt",runin));
	// sers stored in results object
	results sers[NCHANNELS+1];
	
	std::cout << NCHANNELS << std::endl; 	
	for (int i = 1; i <= NCHANNELS; i++) {
		// get ser for channel i-1
		sers[i] = ser_chan[i-1]->get_results();
		// write out to file
		outfile << i-1 << " " << sers[i].xo << " " << sers[i].sigma << " " << sers[i].valley;
		if (i != NCHANNELS) {
			outfile << endl;
		}		
	}
	outfile.close();	
	

/* 
//saves all SER results	 
ofstream out(Form("results_run%d.dat",runin));
		
	results sers[NCHANNELS+1];
	for(int i=1;i<=NCHANNELS;i++)
		{sers[i]=ser_chan[i-1]->get_results();
		out << i-1 << " " << sers[i] << endl;
		cout << i-1 << " " << sers[i] << endl;
		}
	out.close();
*/

/* 
//doesn't work
ofstream wvformsout(Form("wvformresults_run%d.dat",runin));
	for(int j=0;j<250;j++)
		{
		wvformsout << j << " ";
		for(int i=1;i<=NCHANNELS;i++)
			{wvformsout << ser_chan[i-1]->get_histo()->GetBinContent(j) << " "; } 
		wvformsout << endl;
		}
wvformsout.close();
*/
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




















