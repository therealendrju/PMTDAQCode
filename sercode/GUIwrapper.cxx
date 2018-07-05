

#include "GUIwrapper.h"




ClassImp(GUIwrapper)






GUIwrapper::GUIwrapper(int nchans,int in,vector<string> files,  std::map<std::string,double> parameters):NCHANS(nchans),runin(in)
{

 // ,threshold(parameters),before_window(bef),after_window(aft),window(wind)
 threshold=parameters["threshold"];
 before_window=parameters["intbef"]; 
 after_window=parameters["intaft"]; 
 window=parameters["window"]; 
 
 threshold_per_pmt.resize(nchans);
 for (int ix=0;ix<nchans;ix++)
   threshold_per_pmt[ix]=parameters[Form("channel%d",ix)];
 

display_mode=0;

for(unsigned int i=0;i<files.size();i++)
  {filelist.push_back(files[i]);
   cout <<"filelist" << filelist[i] << endl;	
	}
//std::exit(-1);	
// create and connect the main frame.
  fMainFrame = new MainFrame(0,10,10,kMainFrame | kVerticalFrame,runin);

  for(int i=0;i<NCHANS;i++)
	{fMainFrame->get_display_widget(i)->get_refit_button()->Connect("Clicked()","GUIwrapper",this,Form("OpenAux(=%d)",i));
	fMainFrame->get_display_widget(i)->get_check_button()->Connect("Clicked()","PMTwidget",fMainFrame->get_display_widget(i),"toggle_batch_refit()");

	}
 
fMainFrame->get_abort_button()->Connect("Clicked()","GUIwrapper",this,"CloseWindow()");

fMainFrame->get_saveDB_button()->Connect("Clicked()","GUIwrapper",this,"SaveDB()");

fMainFrame->get_savefile_button()->Connect("Clicked()","GUIwrapper",this,"SaveFile()");

fMainFrame->get_batch_button()->Connect("Clicked()","GUIwrapper",this,"BatchFit()");        




fMainFrame->get_ser_button()->Connect("Clicked()","GUIwrapper",this,"ChangeDisplay(=0)");

fMainFrame->get_amp_button()->Connect("Clicked()","GUIwrapper",this,"ChangeDisplay(=1)");

fMainFrame->get_length_button()->Connect("Clicked()","GUIwrapper",this,"ChangeDisplay(=2)");   



//do not initialize the auxiliary frame.
fAuxFrame=NULL; // = new AuxFrame(0,10,10,kMainFrame | kVerticalFrame,2,2);
already_fitted=false;

initialize_data();


}


GUIwrapper::~GUIwrapper()
{

delete fMainFrame;

if(fAuxFrame!=NULL)
	delete fAuxFrame;


}




void GUIwrapper::initialize_data()
{
analysis = new process(NCHANS);


analysis->set_ser_params(threshold,before_window,after_window,window);
		
		

analysis->get_params_from_file();

analysis->read_channels(runin,filelist);

draw_data();
}


void GUIwrapper::draw_data()
{
	
for(int i=0;i<NCHANS;i++)
{
	if(display_mode==0)
		{if(analysis->get_channel(i)->get_histo()!=NULL)
			{fMainFrame->draw_histo_in_pad(analysis->get_channel(i)->get_histo(),i);
			if(!already_fitted)	
				{int status=analysis->fit_channel(i);
				//cout << " ----status "<<i  <<" " << status << endl;
				if(status)
					fMainFrame->get_display_widget(i)->set_channel_status(converged);
				else
					fMainFrame->get_display_widget(i)->set_channel_status(notconverged);
				}
			}
				
		}
		else if(display_mode==1)
			{ if(analysis->get_ampchannel(i)->get_histo()!=NULL)
				{fMainFrame->draw_histo_in_pad(analysis->get_ampchannel(i)->get_histo(),i);
				
				//fMainFrame->get_display_widget(i)->set_channel_status(converged);
					}
				
			}
		else if(display_mode==2)
			{ if(analysis->get_lengthchannel(i)->get_histo()!=NULL)
				{fMainFrame->draw_histo_in_pad(analysis->get_lengthchannel(i)->get_histo(),i);
				
				//fMainFrame->get_display_widget(i)->set_channel_status(converged);
				}
				
			}		
		
		
	}
	
if(display_mode==0)
	already_fitted=true;	
	
	
	
}


void GUIwrapper::OpenAux(int channel)
{

if(fAuxFrame==NULL)
	fAuxFrame=new AuxFrame(0,10,10,kMainFrame | kVerticalFrame,process::NEXP,process::NGAUSS);
// else
// 	{
// 	delete fAuxFrame;
// 	fAuxFrame=new AuxFrame(0,10,10,kMainFrame | kVerticalFrame,2,2);
// 	}
 else
 	{
 	fAuxFrame->get_refit_button()->Disconnect();
	fAuxFrame->get_done_button()->Disconnect();
 	 cout << " disconnecting " <<endl;
  } 
// 
// }
auxchannel=channel;
cout << "In Open aux" << endl;

double *pv=new double[15];
double *pl=new double[15];
double *ph=new double[15];

//get the current values of the fit parameters
analysis->get_param_arrays(pv,pl,ph);

if(display_mode==0)
	fAuxFrame->draw_histo_in_pads(analysis->get_channel(channel)->get_histo(),channel);
else if(display_mode==1)
	fAuxFrame->draw_histo_in_pads(analysis->get_ampchannel(channel)->get_histo(),channel,1);
else if(display_mode==2)
	fAuxFrame->draw_histo_in_pads(analysis->get_lengthchannel(channel)->get_histo(),channel,1);

// and use them to initialize the auxiliary frame
fAuxFrame->set_params_in_frame(pv,pl,ph,fAuxFrame->get_gauss_number());

fAuxFrame->get_refit_button()->Connect("Clicked()","GUIwrapper",this,Form("FitFromAuxWindow(=%d)",channel));
fAuxFrame->get_done_button()->Connect("Clicked()","GUIwrapper",this,"CloseAuxWindow()");

fAuxFrame->get_change_button()->Connect("Clicked()","GUIwrapper",this,"ChangeNGaussInAuxWindow()");


fAuxFrame->SetWindowName(Form("Auxiliary Window, pmt nr %d",channel));
  fAuxFrame->MapSubwindows();
  fAuxFrame->Resize(fAuxFrame->GetDefaultSize());
  fAuxFrame->MapWindow();

//fAuxFrame->MapWindow();


delete pv;
delete pl;
delete ph;
}




void GUIwrapper::CloseWindow()
{
cout << " Closing program " << endl;

if(fAuxFrame!=NULL)
	fAuxFrame->UnmapWindow();

fMainFrame->UnmapWindow();

 gApplication->Terminate();
}

void GUIwrapper::SaveDB()
{
int onoff[NCHANS];

fMainFrame->GetOffWindows(onoff,NCHANS);
	
//analysis->save_results_toDB(runin,onoff);


cout << "saving to DB" << endl;

}



void GUIwrapper::SaveFile()
{

	analysis->save_results_to_file(runin);


	cout << "saving to File" << endl;

}

void GUIwrapper::BatchFit()
{

cout << "batch fit" << endl;


}



void GUIwrapper::ChangeDisplay(int mode)
{
	
	display_mode=mode;
	
	fMainFrame->get_ser_button()->SetOn(kFALSE);
	fMainFrame->get_amp_button()->SetOn(kFALSE);
	fMainFrame->get_length_button()->SetOn(kFALSE);
	if(mode==0)
		{fMainFrame->get_ser_button()->SetOn(kTRUE);}
	else if(mode==1)
		{fMainFrame->get_amp_button()->SetOn(kTRUE);}
	else if(mode==2)
		{fMainFrame->get_length_button()->SetOn(kTRUE);}
	
	draw_data();
	}

void GUIwrapper::CloseAuxWindow()
{

fAuxFrame->UnmapWindow();

}


void GUIwrapper::FitFromAuxWindow(int chan)
{

cout << "fitting channel " << chan << endl;

//hack to catch disconnecting signals
if(chan!=auxchannel)
   return;

analysis->get_params_from_file();


	if(analysis->get_channel(chan)->get_histo()!=NULL)
		{
		double pval[15],plow[15],phigh[15];
		fAuxFrame->get_params_from_frame(pval,plow,phigh);
		analysis->set_param_arrays(pval,plow,phigh);
		int status=analysis->fit_channel(chan);
		
		if(status)
		{// fit converges succesfully, do something
		  std::cout << "Fit Converged Succesfully" << std::endl;
		  
		}
		
		fAuxFrame->draw_histo_in_pads(analysis->get_channel(chan)->get_histo(),chan);
		fMainFrame->draw_histo_in_pad(analysis->get_channel(chan)->get_histo(),chan);
		
		}
	


}


void GUIwrapper::ChangeNGaussInAuxWindow()
{
	int NG=fAuxFrame->get_gauss_number();
	analysis->set_NGAUSS(NG);
	fAuxFrame->redraw_frame_with_gauss(NG);
	OpenAux(auxchannel);
	cout << " changing to NG =   " << NG << endl; 	
	
	}






