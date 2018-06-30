

#include "GUIwrapper.h"




ClassImp(GUIwrapper)



const int NDISPLAYEDCHANS=6;


GUIwrapper::GUIwrapper(int nchans,int in,int evt):NCHANS(nchans),runin(in),event(evt)
{

	analysis=NULL;
// create and connect the main frame.
  fMainFrame = new MainFrame(0,10,10,kMainFrame | kVerticalFrame,NCHANS);
  fCurrentGroup=0;  //current group to be displayed
//  for(int i=0;i<NCHANS;i++)
  for(int i=0;i<NDISPLAYEDCHANS;i++)
	{fMainFrame->get_display_widget(i)->get_closeup_button()->Connect("Clicked()","GUIwrapper",this,Form("OpenAux(=%d)",i));
	//fMainFrame->get_display_widget(i)->get_check_button()->Connect("Clicked()","PMTwidget",fMainFrame->get_display_widget(i),"toggle_batch_refit()");

	}
 
fMainFrame->get_abort_button()->Connect("Clicked()","GUIwrapper",this,"CloseWindow()");

fMainFrame->get_next_button()->Connect("Clicked()","GUIwrapper",this,"NextEvt()");

fMainFrame->get_prev_button()->Connect("Clicked()","GUIwrapper",this,"PrevEvt()");        

fMainFrame->get_event_button()->Connect("Clicked()","GUIwrapper",this,"GetEvtByNumber()");    

fMainFrame->get_integ_button()->Connect("Clicked()","GUIwrapper",this,"DrawIntegrals()"); 

fMainFrame->get_raw_button()->Connect("Clicked()","GUIwrapper",this,"DrawRawData()"); 


for(int i=0;i<(NCHANS/NDISPLAYEDCHANS +(bool)NCHANS%NDISPLAYEDCHANS );i++)
	{fMainFrame->get_field_button(i)->Connect("Clicked()","GUIwrapper",this,Form("DrawGroupData(=%d)",i)); 
	//cout << "connecting group button " << i << endl;
	}

//do not initialize the auxiliary frame.
fAuxFrame=NULL; // = new AuxFrame(0,10,10,kMainFrame | kVerticalFrame,2,2);

display_integs=false;
display_rawdata=false;
//cout << " GUIwrapper inside   " << endl;

//printf("----- GUIwrapper printf");

if(runin != 0)
	display_data(true);


}


GUIwrapper::~GUIwrapper()
{

delete fMainFrame;

if(fAuxFrame!=NULL)
	delete fAuxFrame;


}




void GUIwrapper::display_data(bool change_evt)
{

cout << "at start " << endl;
	if(change_evt)
	{
 		if(analysis==NULL)	
 			analysis = new process(NCHANS);
// 		else
// 		{
// 			delete analysis;
// 			analysis = new process(NCHANS);
// 		}
		analysis->read_channels(runin,event);
	}
//analysis->get_params_from_file();

	//	cout << " ++++ in display data" << runin << " " << event << endl;
	
cout << "after reading " << endl;		

fMainFrame->SetWindowName(Form("WArP 100 liter Waveform display, Run %d, Event %d",runin,event));
//for(int i=0;i<NCHANS;i++)
	DrawGroupData(fCurrentGroup);
	
	
	fMainFrame->draw_sumhisto_in_pad(analysis->get_sum_channel(0)->get_histo(),0);
	
	//plot only the corrected integral. -Luciano
	fMainFrame->draw_sumhisto_in_pad(analysis->get_sum_channel(2)->get_histo(),1);
	//fMainFrame->draw_second_sumhisto_in_pad(analysis->get_sum_channel(1)->get_histo(),1);
	
	
}


void GUIwrapper::DrawGroupData(int group)
{
fCurrentGroup=group;


 
	

int index=group*NDISPLAYEDCHANS;
for(int i=0;i<NDISPLAYEDCHANS;i++)
	{
	
	if(i+index>=NCHANS)
		{
		fMainFrame->draw_histo_in_pad(i);
		continue;
		}
	
	if(analysis->get_channel(i+index)->get_histo()!=NULL)
		{
		fMainFrame->get_display_widget(i)->get_closeup_button()->Disconnect();

		fMainFrame->get_display_widget(i)->get_closeup_button()->Connect("Clicked()","GUIwrapper",this,Form("OpenAux(=%d)",i+index));

		if(!display_rawdata)
		{	
		fMainFrame->draw_histo_in_pad(analysis->get_channel(i+index)->get_histo(),i,"AL");
		fMainFrame->set_channel_label(i,Form(" Channel %d ",i+index));
		//cout << "drawing " << i+index << endl;
		if(display_integs)
			{
			//(TH1F *)analysis->get_channel(i)->get_histo()->SetLineColor(kRed);
			fMainFrame->draw_histo_in_pad(analysis->get_intchannel(i+index)->get_histo(),i,"LSAME");
			}
			//fMainFrame->draw_second_histo_in_pad(analysis->get_intchannel(i)->get_histo(),i);
		}
		else
		{
		fMainFrame->draw_histo_in_pad(analysis->get_raw_channel(i+index)->get_histo(),i,"AL");
		fMainFrame->set_channel_label(i,Form(" Channel %d ",i+index));
		//if(display_integs)
		//	fMainFrame->draw_second_histo_in_pad(analysis->get_intchannel(i)->get_histo(),i);
				
			}	
		
		//for(int j=0; j<2000; j++) {
		//if( (j>10 && j <20 ))
	   //    cout << " .... chan,  histo2 " <<j << " " << analysis->get_channel(i)->get_histo()->GetBinContent(j+1) <<  endl;
		//}
		
		//int status=analysis->fit_channel(i);
		//if(status)
		//	fMainFrame->get_display_widget(i)->set_channel_status(converged);
		//else
		//	fMainFrame->get_display_widget(i)->set_channel_status(notconverged);
		
		
		}
	}







}



void GUIwrapper::OpenAux(int channel)
{

if(fAuxFrame==NULL)
	fAuxFrame=new AuxFrame(0,10,10,kMainFrame | kVerticalFrame);
// else
// 	{
// 	delete fAuxFrame;
// 	fAuxFrame=new AuxFrame(0,10,10,kMainFrame | kVerticalFrame);
// 	}

//cout << "In Open aux" << endl;

//double *pv=new double[15];
//double *pl=new double[15];
//double *ph=new double[15];

//get the current values of the fit parameters
//analysis->get_param_arrays(pv,pl,ph);

if(!display_rawdata)
{
	fAuxFrame->draw_histo_in_pads(analysis->get_channel(channel)->get_histo(),channel,"AL");

	if(display_integs)
		{
		//analysis->get_intchannel(channel)->get_histo()->SetLineColor(kRed);
		fAuxFrame->draw_histo_in_pads(analysis->get_intchannel(channel)->get_histo(),channel,"LSAME");
//		fAuxFrame->draw_second_histo_in_pad(analysis->get_intchannel(channel)->get_histo(),channel);
		
		}
}
else
{
	fAuxFrame->draw_histo_in_pads(analysis->get_raw_channel(channel)->get_histo(),channel,"AL");	
}

analysis->write_ser_windows(channel);		
			
			

// and use them to initialize the auxiliary frame
//fAuxFrame->set_params_in_frame(pv,pl,ph,2);

//fAuxFrame->get_refit_button()->Connect("Clicked()","GUIwrapper",this,Form("FitFromAuxWindow(=%d)",channel));
fAuxFrame->get_done_button()->Connect("Clicked()","GUIwrapper",this,"CloseAuxWindow()");

fAuxFrame->SetWindowName(Form("Auxiliary Window, pmt nr %d",channel));
  fAuxFrame->MapSubwindows();
  fAuxFrame->Resize(fAuxFrame->GetDefaultSize());
  fAuxFrame->MapWindow();

//fAuxFrame->MapWindow();


//delete pv;
//delete pl;
//delete ph;
}




void GUIwrapper::CloseWindow()
{
cout << " Closing program " << endl;

if(fAuxFrame!=NULL)
	fAuxFrame->UnmapWindow();

fMainFrame->UnmapWindow();

 gApplication->Terminate();
}

void GUIwrapper::NextEvt()
{
event++;
display_data(true);
cout << "Displaying next Event" << endl;

}

void GUIwrapper::PrevEvt()
{
if(event>0)
event--;
display_data(true);
cout << "Displaying Previous Event" << endl;


}


void GUIwrapper::GetEvtByNumber()
{
	event=fMainFrame->get_event_number();
	runin=fMainFrame->get_run_number();		
	
	display_data(true);
	
	cout << "Displaying selected event" << endl;	
		
	}


void GUIwrapper::CloseAuxWindow()
{

fAuxFrame->UnmapWindow();

}


void GUIwrapper::DrawIntegrals()
{
display_integs=fMainFrame->get_integ_button()->IsOn();	
	display_data(false);
	
	
	
}	



void GUIwrapper::DrawRawData()
{
	display_rawdata=fMainFrame->get_raw_button()->IsOn();	
	display_data(false);
	
	
	
}	


// void GUIwrapper::FitFromAuxWindow(int chan)
// {
// 
// cout << " channel " << chan << endl;
// 
// analysis->get_params_from_file();
// 
// 
// 	if(analysis->get_channel(chan)->get_histo()!=NULL)
// 		{
// 		double pval[15],plow[15],phigh[15];
// 		//fAuxFrame->get_params_from_frame(pval,plow,phigh);
// 		analysis->set_param_arrays(pval,plow,phigh);
// 		int status=analysis->fit_channel(chan);
// 		
// 		fAuxFrame->draw_histo_in_pads(analysis->get_channel(chan)->get_histo(),chan);
// 		fMainFrame->draw_histo_in_pad(analysis->get_channel(chan)->get_histo(),chan);
// 		
// 		}
// 	
// 
// 
// }






