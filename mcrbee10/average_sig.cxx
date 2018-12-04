#include <stdlib.h>
#include <fstream>
#include <vector>

#include "wep.h"
#include "processor_code.h"
#include "TH1F.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TMath.h"

Double_t expofitf(Double_t *x,Double_t *par) 
{ 
	Double_t arg = 0; 
	arg = -(x[0]*par[1]); 
	Double_t fitval = par[0]*TMath::Exp(arg); 
	return fitval; 
}


int main(int argc, char **argv) 
{
	//FILE *average_file;
	vector<int> sel_run;
	vector<int> sel_event;
	
	
	//inin->IsOpen();
	
	if (argc != 2)
    {
		cout << "Usage: average_sig [filename] " << endl;
		return 1;
    }
	
	cout << "Going to open file " << argv[1] << endl;
	
	ifstream sel_file(argv[1]);
	
	if (sel_file.is_open())
    {
		int nev,nrun,nrow;
		char symbol;
		int counter = 0;
		while (!sel_file.eof())
		{
			sel_file >> symbol >> nrow >> symbol >> nrun >> symbol >> nev >> symbol;
			//cout << symbol << nrow << symbol << nrun << symbol << nev << symbol << endl;
			cout << counter << endl;
			if (sel_run.size() != 0)
			{
				//check that the entry is different from the previous one
				if (sel_run[sel_run.size()-1] != nrun ||
					sel_event[sel_event.size()-1] != nev)
				{
					sel_run.push_back(nrun);
					sel_event.push_back(nev);
				}
			}
			else
			{
				sel_run.push_back(nrun);
				sel_event.push_back(nev);
			}
			counter++;
		}
    }
	sel_file.close();
	if (sel_run.size() != sel_event.size())
    {
		cout << "Problem: dimensions of run and event vectors are different" << endl;
		return 1;
    }
	
	cout << "Read " << sel_run.size() << " events from file " << argv[1] << endl;
	//for (size_t i=0;i<sel_run.size();i++)
	// cout << "Run: " << sel_run[i] << ", Event: " << sel_event[i] << endl;
	
	wep ppp;
	
	//Skips the baseline refinement!
	ppp.refine_baseline(true);

	ppp.set_start(start_ev);
	ppp.add_function(get_baseline);
	ppp.add_function(process_pulse);
	//ppp.add_function(fit_s1);
	ppp.add_function(fill_values);
	
	ppp.isAverage=true;
	
//get first event to extract number of samples
	int found = ppp.get_event(sel_run[0], sel_event[0]);
		 

	const int nsamples = ppp.nSAMPLES;
	const int npmts = ppp.nPMT;
	const float samplingfrequency = 1.; 
	float sumpulse[nsamples];
	float sumpulse_int[nsamples];
        float sumpulse_ch[npmts][nsamples];
        float rawpulse[nsamples];
        float rawpulse_ch[npmts][nsamples];

	//float sumpulse_int[nsamples];


	for (int i=0;i<nsamples;i++)
		{sumpulse[i] = 0;
		 rawpulse[i] = 0;
	  	 sumpulse_int[i]=0;
		for(int ipmt=0;ipmt<npmts;ipmt++)
		  {sumpulse_ch[ipmt][i]=0;
                   rawpulse_ch[ipmt][i]=0;
		   }		
		}

		
	//Event loop
	for(size_t i=0;i<sel_run.size();i++)
	{
		int found = ppp.get_event(sel_run[i], sel_event[i]);
		if (!found) ppp.process_event(); 
		
		int nsamplesData = ppp.nSAMPLES;
		if (nsamplesData != nsamples) 
		{
			cout << "Problem! The number of samples is different from run to run " << endl;
			cout << sel_run[i] << " " << ppp.nSAMPLES << endl;
			return 1;
		}
		float samplingFreqData = ppp.get_sampling();
		if (samplingFreqData != samplingfrequency)
		{
			cout << "Problem! The sampling rate is different from run to run " << endl;
			cout << sel_run[i] << " " << ppp.get_sampling();
			return 1; 
		}
		for (int i=0;i<nsamples;i++)
			{sumpulse[i] += adccsum[i];

			for(int ipmt=0;ipmt<npmts;ipmt++)
				{
				sumpulse_int[i]+=adccsumcorr[i+ipmt*nsamples];
 				rawpulse[i] += adccraw[i+ipmt*nsamples];
				sumpulse_ch[ipmt][i]+=adccsumcorr[i+ipmt*nsamples];
				rawpulse_ch[ipmt][i]+=adccraw[i+ipmt*nsamples];
				}
			}
		}
	
//	//Write sumpulse on a TH1F
	TH1F* histo = new TH1F("histo","histo",nsamples,0,(float)(nsamples*samplingfrequency));
	TH1F* histo3 = new TH1F("integ","integ",nsamples,0,(float)(nsamples*samplingfrequency));
	TH1F* histo_ch[npmts];
	TH1F* rawhisto = new TH1F("rawhisto","rawhisto",nsamples,0,(float)(nsamples*samplingfrequency));
	TH1F* rawhisto_ch[npmts];	    


        for(int ipmt=0;ipmt<npmts;ipmt++)
		{ histo_ch[ipmt]= new TH1F(Form("integ_%d",ipmt),Form("integ_%d",ipmt),nsamples,0,(float)(nsamples*samplingfrequency));
		 rawhisto_ch[ipmt]= new TH1F(Form("rawinteg_%d",ipmt),Form("rawinteg_%d",ipmt),nsamples,0,(float)(nsamples*samplingfrequency));
		}
	for (int i=1;i<=nsamples;i++)
    {
		histo->SetBinContent(i,sumpulse[i-1]);
		rawhisto->SetBinContent(i,rawpulse[i-1]);
		histo3->SetBinContent(i,sumpulse_int[i-1]);
		for(int ipmt=0;ipmt<npmts;ipmt++)
			{
		 histo_ch[ipmt]->SetBinContent(i,sumpulse_ch[ipmt][i-1]);
		rawhisto_ch[ipmt]->SetBinContent(i,rawpulse_ch[ipmt][i-1]);
		}
    }
	
	//TF1 *fit_func=new TF1("fit_func","expo",0.,nsamples*samplingfrequency); 
	//histo->Fit("fit_func","ILEM","",1500.,nsamples*samplingfrequency);
	//TCanvas *c001 = new TCanvas("c001","Average Signal",+0,+0,900,600);
	//TF1 *fit_func=new TF1("fit_func","pol0+expo(1)",0.,nsamples*samplingfrequency); 
	//fit_func->SetLineColor(kRed);
	//histo->Draw("e");
	//histo->Fit("fit_func","IL","",5100.,12000.);
	//printf("Slope=%f\n",1./(fit_func->GetParameter(2)));
	//TString cfilename = (TString) argv[1] + ".C";
	//c001->Print(cfilename);
	
	TString rootfilename = (TString) argv[1] + ".root";
	TFile* outputfile = new TFile(rootfilename,"RECREATE");
	if (outputfile->IsOpen())
    {
		outputfile->cd();
		histo->Write(histo->GetName());
                rawhisto->Write(rawhisto->GetName());
		histo3->Write(histo3->GetName());
		for(int ipmt=0;ipmt<npmts;ipmt++)
		 {histo_ch[ipmt]->Write(histo_ch[ipmt]->GetName());
		 rawhisto_ch[ipmt]->Write(rawhisto_ch[ipmt]->GetName());	
		 }
    }
	outputfile->Close();
	
	cout << "Processed: " << sel_run.size() << " events" << endl;
	
	return 0;
}

