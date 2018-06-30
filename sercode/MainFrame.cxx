// Mainframe macro generated from application: /usr/bin/root.exe
// By ROOT version 5.17/05 on 2009-05-09 11:20:32



#include "Riostream.h"


#include "MainFrame.h"


ClassImp(MainFrame)



MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h,UInt_t options,int run) : TGMainFrame(p,w,h,options),runin(run)
{

   // main frame
   
 //  SetLayoutBroken(kTRUE);
	
   // tab widget
	TGHorizontalFrame *hframehi= new TGHorizontalFrame(this,300,300);
	
	
   TGTab *fTab = new TGTab(hframehi,832,384);

   // container of "Tab1"

   fCompositeGroup[0] = fTab->AddTab("Group 1");
   fCompositeGroup[0]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[0]));


   // container of "Tab2"
  
   fCompositeGroup[1] = fTab->AddTab("Group 2");
   fCompositeGroup[1]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[1]));


   // container of "Group 3"
   fCompositeGroup[2] = fTab->AddTab("Group 3");
   fCompositeGroup[2]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[2]));


   // container of "Group 4"

   fCompositeGroup[3]= fTab->AddTab("Group 4");
   fCompositeGroup[3]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[3]));

   fCompositeGroup[4]= fTab->AddTab("Group 5");
   fCompositeGroup[4]->SetLayoutManager(new TGVerticalLayout(fCompositeGroup[4]));	

//    for(int i=0;i<9;i++)
// 	{
// 	displays[i]=new PMTwidget(fCompositeGroup1,100,100);
//         fCompositeGroup1->AddFrame((TGFrame *)displays[i], new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX |kLHintsExpandY,2,2,2,2));
// 	}	

//setting up the displays.

////////first group.

const int NVER=3,NHOR=3,NNFIL=9;
  
TGHorizontalFrame *hframemain[NVER*5]={NULL};


int flag=0;

for(int group=0;group<5;group++)
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
			 displays[i*NHOR+j+group*NNFIL]->set_channel_status(pending);
			 
			 int chan_conv=0;
// 			 if(runin%2)   //odd runs, i.e. warpaq03
// 			 	{
// 					if((i*NHOR+j+group*NNFIL+1)<=8)
// 						chan_conv=8;
// 					else if((i*NHOR+j+group*NNFIL+1)>8)
// 						chan_conv=16;
// 				 }
// 				 else if(!(runin%2))   //even runs, i.e. warpaq04
// 			 	{
// 					 if((i*NHOR+j+group*NNFIL+1)<=8)
// 						 chan_conv=0;
// 					 else if((i*NHOR+j+group*NNFIL+1)>8)
// 						 chan_conv=8;
// 					//
// 				}
					 				 
				displays[i*NHOR+j+group*NNFIL]->set_channel_number(i*NHOR+j+group*NNFIL+chan_conv);	
     			  hframemain[i+group*NVER]->AddFrame((TGFrame *)displays[i*NHOR+j+group*NNFIL],new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));
			
			}
          if(hframemain[i+group*NVER]!=NULL)
	{     	fCompositeGroup[group]->AddFrame(hframemain[i+group*NVER],new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsExpandY,2,2,2,2));}
	//  std::cout << " hframe add " << i+group*NVER << std::endl;}}
	
    }
}






   fTab->SetTab(0);

   fTab->Resize(fTab->GetDefaultSize());
   hframehi->AddFrame(fTab, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX | kLHintsExpandY,2,2,2,2));
   fTab->MoveResize(24,56,832,384);

   
   /////
   
   TGVerticalFrame *vframeright= new TGVerticalFrame(hframehi,300,300);


   fserButton = new TGTextButton(vframeright,"SER");
   fserButton->SetTextJustify(36);
   fserButton->SetMargins(0,0,0,0);
   fserButton->SetWrapLength(-1);
   fserButton->Resize(99,24);
   vframeright->AddFrame(fserButton, new TGLayoutHints(kLHintsRight | kLHintsTop |kLHintsExpandX,2,2,2,2));
   fserButton->SetOn();
   
   fampButton = new TGTextButton(vframeright,"amplitudes");
   fampButton->SetTextJustify(36);
   fampButton->SetMargins(0,0,0,0);
   fampButton->SetWrapLength(-1);
   fampButton->Resize(99,24);
   vframeright->AddFrame(fampButton, new TGLayoutHints(kLHintsRight | kLHintsTop |kLHintsExpandX,2,2,2,2));
   
   flengthButton = new TGTextButton(vframeright,"length");
   flengthButton->SetTextJustify(36);
   flengthButton->SetMargins(0,0,0,0);
   flengthButton->SetWrapLength(-1);
   flengthButton->Resize(99,24);
   vframeright->AddFrame(flengthButton, new TGLayoutHints(kLHintsRight | kLHintsTop |kLHintsExpandX,2,2,2,2));
   
   
   hframehi->AddFrame(vframeright, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandY,2,2,2,2));
   
   
   this->AddFrame(hframehi, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX | kLHintsExpandY,2,2,2,2));
// std::cout << displays[3]->get_channel_status() << std::endl;
// displays[3]->set_channel_status(converged);
// displays[7]->set_channel_status(notconverged);
// std::cout << displays[3]->get_channel_status() << std::endl;

set_all_status(pending);

// hframemain[1]->MapSubwindows();
// hframemain[1]->MapWindow();
// hframemain[1]->Layout();
// fTab->MapSubwindows();
// fTab->MapWindow();   


TGHorizontalFrame *hframelow= new TGHorizontalFrame(this,300,300);

   fAbortButton = new TGTextButton(hframelow,"Abort");
   fAbortButton->SetTextJustify(36);
   fAbortButton->SetMargins(0,0,0,0);
   fAbortButton->SetWrapLength(-1);
   fAbortButton->Resize(99,24);
   hframelow->AddFrame(fAbortButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fAbortButton->MoveResize(776,504,99,24);
   fSaveDBButton = new TGTextButton(hframelow,"Save to DB");
   fSaveDBButton->SetTextJustify(36);
   fSaveDBButton->SetMargins(0,0,0,0);
   fSaveDBButton->SetWrapLength(-1);
   fSaveDBButton->Resize(99,24);
   hframelow->AddFrame(fSaveDBButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fSaveDBButton->MoveResize(664,504,99,24);
   
   
   fSaveFileButton = new TGTextButton(hframelow,"Save to file");
   fSaveFileButton->SetTextJustify(36);
   fSaveFileButton->SetMargins(0,0,0,0);
   fSaveFileButton->SetWrapLength(-1);
   fSaveFileButton->Resize(99,24);
   hframelow->AddFrame(fSaveFileButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fSaveFileButton->MoveResize(664,504,99,24);
   
   
   
   fBatchButton = new TGTextButton(hframelow,"Batch refit");
   fBatchButton->SetTextJustify(36);
   fBatchButton->SetMargins(0,0,0,0);
   fBatchButton->SetWrapLength(-1);
   fBatchButton->Resize(99,24);
   hframelow->AddFrame(fBatchButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fBatchButton->MoveResize(544,504,99,24);

  AddFrame(hframelow, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
   


   SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   MapSubwindows();
   SetWindowName(Form("WArP 100 liter SER display, run nr %d",runin));
   Resize(this->GetDefaultSize());
   MapWindow();
   Resize(907,547);
}  




MainFrame::~MainFrame()
{


int a=1;

}


void MainFrame::draw_histo_in_pad(TH1F * histo,int pad)
{

if(pad>NPMTS)
	{
	std::cout << "NO PMT NUMBER " << pad << std::endl; 
	return;
	}

displays[pad]->get_canvas()->cd();

if(histo!=NULL)
	histo->DrawCopy();
//cout << "++ histo name "<< histo->GetName() << endl;
	
displays[pad]->get_canvas()->Update();


}

void MainFrame::set_all_status(int status)
{

for(int i=0;i<NPMTS;i++)
	displays[i]->set_channel_status(status);


}


PMTwidget * MainFrame::get_display_widget(int number)
{
if(number<NPMTS)
return displays[number];
else
return NULL;

}


TGTextButton * MainFrame::get_abort_button()
{
return fAbortButton;
}

TGTextButton * MainFrame::get_saveDB_button()
{
return fSaveDBButton;
}


TGTextButton * MainFrame::get_savefile_button()
{
	return fSaveFileButton;
}


TGTextButton * MainFrame::get_batch_button()
{
return fBatchButton;
}


TGTextButton * MainFrame::get_ser_button()
{
	return fserButton;
}

TGTextButton * MainFrame::get_amp_button()
{
	return fampButton;
}


TGTextButton * MainFrame::get_length_button()
{
	return flengthButton;
}



void MainFrame::GetOffWindows(int *onoff,int nchans)
{
	for(int i=0;i<nchans;i++)
		onoff[i]=!displays[i]->get_batch_refit();
	
	
	}



