// Mainframe macro generated from application: /usr/bin/root.exe
// By ROOT version 5.17/05 on 2009-05-09 11:20:32



#include "Riostream.h"


#include "MainFrame.h"


ClassImp(MainFrame)



MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h,UInt_t options,int NCHANNELS) : TGMainFrame(p,w,h,options)
{

   // main frame
   
 //  SetLayoutBroken(kTRUE);

   // tab widget
   TGHorizontalFrame *hframe=new TGHorizontalFrame(this,300,300);
	
   
   TGTab *fTab = new TGTab(hframe,832,384);

   // container of "Tab1"

    fCompositeGroup[1] = fTab->AddTab("Sum of PMTs");
   fCompositeGroup[1]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[1]));
   
   
   fCompositeGroup[0] = fTab->AddTab("Group 1");
   fCompositeGroup[0]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[0]));


   // container of "Tab2"
  
   //fCompositeGroup[1] = fTab->AddTab("Group 2");
   //fCompositeGroup[1]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[1]));


   // container of "Group 3"
   //fCompositeGroup[2] = fTab->AddTab("Group 3");
   //fCompositeGroup[2]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[2]));


   // container of "Group 4"

   //fCompositeGroup[3]= fTab->AddTab("Group 4");
   //fCompositeGroup[3]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[3]));

   //fCompositeGroup[4]= fTab->AddTab("Group 5");
   //fCompositeGroup[4]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[4]));	

//    for(int i=0;i<9;i++)
// 	{
// 	displays[i]=new PMTwidget(fCompositeGroup1,100,100);
//         fCompositeGroup1->AddFrame((TGFrame *)displays[i], new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX |kLHintsExpandY,2,2,2,2));
// 	}	

//setting up the displays.

//sumgroup

TGHorizontalFrame *hfram= new TGHorizontalFrame(fCompositeGroup[1],300,300);


sumdisplays[0] = new TRootEmbeddedCanvas(0,hfram,440,192);
   Int_t wfEmbeddedCanvas = sumdisplays[0]->GetCanvasWindowId();
   canvas[0] = new TCanvas("canvas", 10, 10, wfEmbeddedCanvas);
   sumdisplays[0]->AdoptCanvas(canvas[0]);
   hfram->AddFrame(sumdisplays[0], new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX |kLHintsExpandY,2,2,2,2));
   sumdisplays[0]->MoveResize(32,64,440,192);

//hfram->AddFrame(sumdisplays[0],new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));


sumdisplays[1] = new TRootEmbeddedCanvas(0,hfram,440,192);
   Int_t wfEmbeddedCanvas2 = sumdisplays[1]->GetCanvasWindowId();
   canvas[1] = new TCanvas("canvas", 10, 10, wfEmbeddedCanvas2);
   sumdisplays[1]->AdoptCanvas(canvas[1]);
   hfram->AddFrame(sumdisplays[1], new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX |kLHintsExpandY,2,2,2,2));
   sumdisplays[1]->MoveResize(32,64,440,192);



//hfram->AddFrame(sumdisplays[1],new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));
fCompositeGroup[1]->AddFrame(hfram,new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY,2,2,2,2));

////////first group.

const int NVER=3,NHOR=2,NNFIL=6;
  
TGHorizontalFrame *hframemain[NVER*5]={NULL};


int flag=0;

for(int group=0;group<1;group++)
 {
 	for(Int_t i=0;i<NVER;i++)
    	{
	  if(!flag)
	   {hframemain[i+group*NVER]= new TGHorizontalFrame(fCompositeGroup[group],300,300);}
      	  // std::cout << " hframe create " << i+group*NVER << std::endl;}}
		for(Int_t j=0;j<NHOR;j++)
			{
			if((j+i*NHOR)>=(NNFIL)) break;
			if((j+i*NHOR)+group*NNFIL>=(NPMTS))
				{ flag=1;
				break;}
			
	 		 displays[i*NHOR+j+group*NNFIL]=new PMTwidget(hframemain[i+group*NVER],100,100);
			//std::cout << "i,j,group " << i <<" "<<j<<" "<<group <<" " << i*NHOR+j+group*NNFIL <<" frame  " << i+group*NVER << std::endl;	  
			 //displays[i*NHOR+j+group*NNFIL]->set_channel_status(pending);	
			 displays[i*NHOR+j+group*NNFIL]->set_channel_number(i*NHOR+j+group*NNFIL);	
     			  hframemain[i+group*NVER]->AddFrame((TGFrame *)displays[i*NHOR+j+group*NNFIL],new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));
			
			}
          if(hframemain[i+group*NVER]!=NULL)
	{     	fCompositeGroup[group]->AddFrame(hframemain[i+group*NVER],new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY,2,2,2,2));}
	//  std::cout << " hframe add " << i+group*NVER << std::endl;}}
	
    }
}






   fTab->SetTab(0);

   fTab->Resize(fTab->GetDefaultSize());
   hframe->AddFrame(fTab, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   fTab->MoveResize(24,56,832,384);

   
   // right vertical frame

   TGVerticalFrame *vframe=new TGVerticalFrame(hframe,300,300);
   
   
   
   
   
   fRunEntry = new TGNumberEntryField(vframe, 1, 0,
                                    TGNumberFormat::kNESReal,
                                    TGNumberFormat::kNEAAnyNumber,
                                    TGNumberFormat::kNELNoLimits);
				      
   fRunEntry->SetNumber(0);
     			     
   TString *temp=new TString("Run number ");
   fRunEntry->SetToolTipText(temp->Data());
   vframe->AddFrame(fRunEntry, new TGLayoutHints(kLHintsRight | kLHintsTop, 18, 1, 1, 1));
				     
   delete temp;
				     
   
   
   fEvtEntry = new TGNumberEntryField(vframe, 1, 0,
				      TGNumberFormat::kNESReal,
				      TGNumberFormat::kNEAAnyNumber,
				      TGNumberFormat::kNELNoLimits);
				      
   fEvtEntry->SetNumber(0);
     			     
   temp=new TString("Event number ");
   fEvtEntry->SetToolTipText(temp->Data());
   vframe->AddFrame(fEvtEntry, new TGLayoutHints(kLHintsRight | kLHintsTop , 18, 1, 1, 1));
				     
   delete temp;

   
    
   
   
   
   
//   TGTextButton *fEventButton;
   fEventButton = new TGTextButton(vframe,"Get Event");
   fEventButton->SetTextJustify(36);
   fEventButton->SetMargins(0,0,0,0);
   fEventButton->SetWrapLength(-1);
   fEventButton->Resize(99,24);
   vframe->AddFrame(fEventButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fEventButton->MoveResize(776,504,99,24);
   
   fIntegButton = new TGCheckButton(vframe,"show integral");
   fIntegButton->SetToolTipText("mark to show integral windows");
   fIntegButton->SetTextJustify(36);
   fIntegButton->SetMargins(0,0,0,0);
   fIntegButton->SetWrapLength(-1);
   vframe->AddFrame(fIntegButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fIntegButton->MoveResize(48,336,128,32);
   
   fRawButton = new TGCheckButton(vframe,"show raw data");
   fRawButton->SetToolTipText("mark to show raw data instead of cleaned signals");
   fRawButton->SetTextJustify(36);
   fRawButton->SetMargins(0,0,0,0);
   fRawButton->SetWrapLength(-1);
   vframe->AddFrame(fRawButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRawButton->MoveResize(48,336,128,32);
   
   fNbuttons=0;
   
   for(int xx=0;xx*NNFIL<=NCHANNELS;xx++)
      {
    //  cout << " button 1 " << xx << " " << xx+NNFIL << endl;
      int start_chan=xx*NNFIL;
      int stop_chan = ((xx+1)*NNFIL > NCHANNELS) ? NCHANNELS : (xx+1)*NNFIL;
    //  cout << " button "<< xx << " " << start_chan << " " << stop_chan << endl;
      
        fGroupButton[xx] = new TGTextButton(vframe,Form("PMTS %d-%d",start_chan,stop_chan));
        fGroupButton[xx]->SetTextJustify(36);
        fGroupButton[xx]->SetMargins(0,0,0,0);
        fGroupButton[xx]->SetWrapLength(-1);
        fGroupButton[xx]->Resize(99,24);
        vframe->AddFrame(fGroupButton[xx], new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
        fGroupButton[xx]->MoveResize(776,504,99,24);
      
      fNbuttons++;
      
      }
  
//    
   
   
   
   
   
   
   
   hframe->AddFrame(vframe, new TGLayoutHints(kLHintsLeft | kLHintsBottom | kLHintsExpandY,2,2,2,2));
   

   this->AddFrame(hframe, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX | kLHintsExpandY,2,2,2,2));
// std::cout << displays[3]->get_channel_status() << std::endl;
// displays[3]->set_channel_status(converged);
// displays[7]->set_channel_status(notconverged);
// std::cout << displays[3]->get_channel_status() << std::endl;

//set_all_status(pending);

// hframemain[1]->MapSubwindows();
// hframemain[1]->MapWindow();
// hframemain[1]->Layout();
// fTab->MapSubwindows();
// fTab->MapWindow();   


TGHorizontalFrame *hframelow= new TGHorizontalFrame(this,300,300);

   fAbortButton = new TGTextButton(hframelow,"Quit");
   fAbortButton->SetTextJustify(36);
   fAbortButton->SetMargins(0,0,0,0);
   fAbortButton->SetWrapLength(-1);
   fAbortButton->Resize(99,24);
   hframelow->AddFrame(fAbortButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fAbortButton->MoveResize(776,504,99,24);
   fNextButton = new TGTextButton(hframelow,"Next Event");
   fNextButton->SetTextJustify(36);
   fNextButton->SetMargins(0,0,0,0);
   fNextButton->SetWrapLength(-1);
   fNextButton->Resize(99,24);
   hframelow->AddFrame(fNextButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fNextButton->MoveResize(664,504,99,24);
   fPrevButton = new TGTextButton(hframelow,"Previous Event");
   fPrevButton->SetTextJustify(36);
   fPrevButton->SetMargins(0,0,0,0);
   fPrevButton->SetWrapLength(-1);
   fPrevButton->Resize(99,24);
   hframelow->AddFrame(fPrevButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fPrevButton->MoveResize(544,504,99,24);

  AddFrame(hframelow, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
   


   SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   MapSubwindows();
   SetWindowName("WArP 100 liter Waveform display");
   Resize(this->GetDefaultSize());
   MapWindow();
   Resize(907,547);
}  




MainFrame::~MainFrame()
{


int a=1;

}


void MainFrame::draw_histo_in_pad(TObject * histo,int pad,TString option)
{

if(pad>NPMTS)
	{
	std::cout << "NO PMT NUMBER " << pad << std::endl; 
	return;
	}

displays[pad]->get_canvas()->cd();

if(histo!=NULL)
	histo->Draw(option);
//std::cout << " (((((((( drawing histo number " << pad << std::endl; 
	
displays[pad]->get_canvas()->Update();


}


void MainFrame::draw_histo_in_pad(int pad)
{
if(pad>NPMTS)
	{
	std::cout << "NO PMT NUMBER " << pad << std::endl; 
	return;
	}
displays[pad]->get_canvas()->cd();

displays[pad]->get_canvas()->Clear();
displays[pad]->set_channel_label("    none    ");
displays[pad]->get_canvas()->Update();
}



 void MainFrame::set_channel_label(int pad,TString text)
 {
 displays[pad]->set_channel_label(text);
  
 }


// void MainFrame::draw_second_histo_in_pad(TObject * histo,int pad)
// {
// 
// 	if(pad>NPMTS)
// 	{
// 		std::cout << "NO PMT NUMBER " << pad << std::endl; 
// 		return;
// 	}
// 
// 	displays[pad]->get_canvas()->cd();
// 
// 	if(histo!=NULL)
// 	{
// 		//histo->SetLineColor(kRed);
// 		histo->Draw("same");
// 		
// 		}
// 	//std::cout << " (((((((( drawing histo number " << pad << std::endl; 
// 	
// 	displays[pad]->get_canvas()->Update();
// 
// 
// }





void MainFrame::draw_sumhisto_in_pad(TObject * histo,int pad)
{

sumdisplays[pad]->GetCanvas()->cd();
if(histo!=NULL)
	histo->Draw();

		
sumdisplays[pad]->GetCanvas()->Update();


}



void MainFrame::draw_second_sumhisto_in_pad(TObject * histo2,int pad)
{

	sumdisplays[pad]->GetCanvas()->cd();
	
	if(histo2!=NULL)
		{
		//histo2->SetLineColor(kRed);	
		histo2->Draw("same");

		}
		
	sumdisplays[pad]->GetCanvas()->Update();


}









// void MainFrame::set_all_status(int status)
// {
// 
// for(int i=0;i<NPMTS;i++)
// 	displays[i]->set_channel_status(status);
// 
// 
// }


PMTwidget * MainFrame::get_display_widget(int number)
{
if(number<NPMTS)
return displays[number];
else
return NULL;

}


TRootEmbeddedCanvas * MainFrame::get_sumdisplay_canvas(int number)
{
if(number<2)
return sumdisplays[number];
else
return NULL;

}




TGTextButton * MainFrame::get_abort_button()
{
return fAbortButton;
}

TGTextButton * MainFrame::get_next_button()
{
return fNextButton;
}


TGTextButton * MainFrame::get_prev_button()
{
return fPrevButton;
}


TGTextButton * MainFrame::get_event_button()
{
	return fEventButton;
}

TGTextButton * MainFrame::get_field_button(int button)
{
	if(button<=fNbuttons)
		return fGroupButton[button];

return NULL;
}


 TGCheckButton *MainFrame::get_integ_button()
{
	return fIntegButton;
	}

	
TGCheckButton *MainFrame::get_raw_button()
{
	return fRawButton;
	}

	
	
int MainFrame::get_run_number()
{
	return fRunEntry->GetNumber(); 
	 
   
	
	}	
		
		
int MainFrame::get_event_number()
{
	return fEvtEntry->GetNumber();
	
	}

