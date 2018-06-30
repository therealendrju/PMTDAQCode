#include <stdlib.h>
#include <fstream> 

#include "wed.h"
#include "processor_code.h"
#include "TMath.h"

enum ETestCommandIdentifiers {
  M_FILE_OPEN,
  M_HIST_DRAW,
  M_FILE_EXIT,
  M_PMT_ALL_1,
  M_PMT_ALL_2,
  M_PMT_ALL_3,
  M_PMT_ALL_4,
  M_PMT_ALL_5,
  M_PMT_1,
  M_PMT_2,
  M_PMT_3,
  M_PMT_4,
  M_PMT_5,
  M_PMT_6,
  M_PMT_7,
  M_PMT_8,
  M_PMT_9,
  M_PMT_10,
  M_PMT_11,
  M_PMT_12,
  M_PMT_13,
  M_PMT_14,
  M_PMT_15,
  M_PMT_16,
  M_PMT_17,
  M_PMT_18,
  M_PMT_19,
  M_PMT_20,
  M_PMT_21,
  M_PMT_22,
  M_PMT_23,
  M_PMT_24,
  M_PMT_25,
  M_PMT_26,
  M_PMT_27,
  M_PMT_28,
  M_PMT_29,
  M_PMT_30,
  M_PMT_31,
  M_PMT_32,
  M_PMT_33,
  M_PMT_34,
  M_PMT_35,
  M_PMT_36,
  M_PMT_37,
  M_PMT_38
};

const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            0,               0 };

Double_t aaa=0.0;
Double_t aab=0.0;

extern void make_fit(float*);
extern my_event the_event;
//extern int pmtthreshold[];

WEDMainFrame::WEDMainFrame(wep *ptr, const TGWindow *p, UInt_t w, UInt_t h)
  : TGMainFrame(p, w, h)
{
  // processor pointer
  ppp = ptr;

  file_is_open = false;
  show_integ_win = false;
  current_pmt = 0;
  last_view = 0;
  pmt_page = 1;

  // Create test main frame. A TGMainFrame is a top level window.

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);

  // Create menubar and popup menus. The hint objects are used to place
  // and group the different menu widgets with respect to eachother.
  //    fTop = new TGCompositeFrame(this);
  fTop = new TGVerticalFrame(this, w, h);
  AddFrame(fTop, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 0));
  fTop->SetWindowName("menu");

  // File menu popup

  fMenuFile = new TGPopupMenu(fClient->GetRoot());
  fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
  fMenuFile->AddEntry("&Draw", M_HIST_DRAW);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

  fMenuFile->Associate(this);

  // PMT menu popup

  fMenuPMT1 = new TGPopupMenu(fClient->GetRoot());
  fMenuPMT1->AddEntry("PMT 1", M_PMT_1);
  fMenuPMT1->AddEntry("PMT 2", M_PMT_2);
  fMenuPMT1->AddEntry("PMT 3", M_PMT_3);
  fMenuPMT1->AddEntry("PMT 4", M_PMT_4);
  fMenuPMT1->AddEntry("PMT 5", M_PMT_5);
  fMenuPMT1->AddEntry("PMT 6", M_PMT_6);
  fMenuPMT1->AddEntry("PMT 7", M_PMT_7);
  fMenuPMT1->AddEntry("PMT 8", M_PMT_8);
  fMenuPMT1->AddSeparator();
  fMenuPMT1->AddEntry("ALL1", M_PMT_ALL_1);

  fMenuPMT1->Associate(this);

  fMenuPMT2 = new TGPopupMenu(fClient->GetRoot());
  fMenuPMT2->AddEntry("PMT 9 ", M_PMT_9 );
  fMenuPMT2->AddEntry("PMT 10", M_PMT_10);
  fMenuPMT2->AddEntry("PMT 11", M_PMT_11);
  fMenuPMT2->AddEntry("PMT 12", M_PMT_12);
  fMenuPMT2->AddEntry("PMT 13", M_PMT_13);
  fMenuPMT2->AddEntry("PMT 14", M_PMT_14);
  fMenuPMT2->AddEntry("PMT 15", M_PMT_15);
  fMenuPMT2->AddEntry("PMT 16", M_PMT_16);
  fMenuPMT2->AddSeparator();
  fMenuPMT2->AddEntry("ALL2", M_PMT_ALL_2);

  fMenuPMT2->Associate(this);

  fMenuPMT3 = new TGPopupMenu(fClient->GetRoot());
  fMenuPMT3->AddEntry("PMT 17", M_PMT_17);
  fMenuPMT3->AddEntry("PMT 18", M_PMT_18);
  fMenuPMT3->AddEntry("PMT 19", M_PMT_19);
  fMenuPMT3->AddEntry("PMT 20", M_PMT_20);
  fMenuPMT3->AddEntry("PMT 21", M_PMT_21);
  fMenuPMT3->AddEntry("PMT 22", M_PMT_22);
  fMenuPMT3->AddEntry("PMT 23", M_PMT_23);
  fMenuPMT3->AddEntry("PMT 24", M_PMT_24);
  fMenuPMT3->AddSeparator();
  fMenuPMT3->AddEntry("ALL3", M_PMT_ALL_3);

  fMenuPMT3->Associate(this);

  fMenuPMT4 = new TGPopupMenu(fClient->GetRoot());
  fMenuPMT4->AddEntry("PMT 25", M_PMT_25);
  fMenuPMT4->AddEntry("PMT 26", M_PMT_26);
  fMenuPMT4->AddEntry("PMT 27", M_PMT_27);
  fMenuPMT4->AddEntry("PMT 28", M_PMT_28);
  fMenuPMT4->AddEntry("PMT 29", M_PMT_29);
  fMenuPMT4->AddEntry("PMT 30", M_PMT_30);
  fMenuPMT4->AddEntry("PMT 31", M_PMT_31);
  fMenuPMT4->AddEntry("PMT 32", M_PMT_32);
  fMenuPMT4->AddSeparator();
  fMenuPMT4->AddEntry("ALL4", M_PMT_ALL_4);

  fMenuPMT4->Associate(this);

  fMenuPMT5 = new TGPopupMenu(fClient->GetRoot());
  fMenuPMT5->AddEntry("PMT 33", M_PMT_33);
  fMenuPMT5->AddEntry("PMT 34", M_PMT_34);
  fMenuPMT5->AddEntry("PMT 35", M_PMT_35);
  fMenuPMT5->AddEntry("PMT 36", M_PMT_36);
  fMenuPMT5->AddEntry("PMT 37", M_PMT_37);
  fMenuPMT5->AddEntry("PMT 38", M_PMT_38);
  fMenuPMT5->AddSeparator();
  fMenuPMT5->AddEntry("ALL5", M_PMT_ALL_5);

  fMenuPMT5->Associate(this);

  // -----

  fMenuBar = new TGMenuBar(fTop, 1, 1, kHorizontalFrame);
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
  fMenuBar->AddPopup("PMT  1- 8",  fMenuPMT1, fMenuBarItemLayout);
  fMenuBar->AddPopup("PMT  9-16",  fMenuPMT2, fMenuBarItemLayout);
  fMenuBar->AddPopup("PMT 17-24",  fMenuPMT3, fMenuBarItemLayout);
  fMenuBar->AddPopup("PMT 25-32",  fMenuPMT4, fMenuBarItemLayout);
  fMenuBar->AddPopup("PMT 33-37",  fMenuPMT5, fMenuBarItemLayout);

  fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  fTop->AddFrame(fMenuBar, fMenuBarLayout); // <---

  // -----

  fMain = new TGHorizontalFrame(fTop, w, h-50);
  fTop->AddFrame(fMain, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 0));

  fDisplay = new TGVerticalFrame(fMain, w-100, h-50);
  fMain->AddFrame(fDisplay, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 0));

  fInfo = new TGVerticalFrame(fMain, 100, h-50);
  fMain->AddFrame(fInfo, new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandY, 0, 0, 1, 0));

  fLabelRun = new TGLabel(fInfo, new TGString("Run# "));
  fInfo->AddFrame(fLabelRun, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 1, 0));

  fLabelEvent = new TGLabel(fInfo, new TGString("Event# "));
  fInfo->AddFrame(fLabelEvent, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 1, 0));

  fNext = new TGTextButton(fInfo, "Next event", 154);
  fNext->Associate(this);
  fInfo->AddFrame(fNext, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 1, 0));

  fRun = new TGNumberEntry(fInfo, aaa, 10, 161, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
  fRun->Associate(this);
  fInfo->AddFrame(fRun, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 1, 0));
 
  fEvent = new TGNumberEntry(fInfo, aab, 10, 162, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
  fEvent->Associate(this);
  fInfo->AddFrame(fEvent, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 1, 0));

  fInte = new TGCheckButton(fInfo, "IntegWin", 155);
  fInte->Associate(this);
  fInfo->AddFrame(fInte, new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,0, 0, 10, 0));


  fCanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
				    kLHintsExpandY, 5, 5, 5, 5);

  fCanvas1 = new TRootEmbeddedCanvas("data_canvas", fDisplay, 1200, 300);
  fDisplay->AddFrame(fCanvas1, fCanvasLayout);

  fCanvas2 = new TRootEmbeddedCanvas("proc_canvas", fDisplay, 1200, 300);
  fDisplay->AddFrame(fCanvas2, fCanvasLayout);

  //Here it is the 
  fTCanvas1 = fCanvas1->GetCanvas();
  fTCanvas1->Divide(2,4); //2 columns, 4 rows

  fTCanvas1->Modified();
  fTCanvas1->Update();

  fTCanvas2 = fCanvas2->GetCanvas();
  fTCanvas2->Divide(2,2);

  fTCanvas2->Modified();
  fTCanvas2->Update();

  gSystem->ProcessEvents();  // handle GUI events

  SetWindowName("Warp Event Display and Analysis Tool");
  MapSubwindows();

  // we need to use GetDefault...() to initialize the layout algorithm...
  Resize();   // resize to default size
  MapWindow();
  Print();
   
  //First ones are for data, then for analysis 
  for (size_t i=0;i<40;i++) {
    fHist[i] = NULL;
    fHistIntegWin[i] = NULL;
  }
  for (size_t i=0;i<4;i++)
    fHistAnalysis[i] = NULL;
 
  myf = NULL;
}

WEDMainFrame::~WEDMainFrame() {

  delete fMenuFile;

}

void WEDMainFrame::CloseWindow()
{
  // Got close message for this MainFrame. Terminate the application
  // or returns from the TApplication event loop (depending on the
  // argument specified in TApplication::Run()).

  gApplication->Terminate(0);
}

Bool_t WEDMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
  // Handle messages send to the WEDMainFrame object. E.g. all menu button
  // messages.
  //cout << "Sono qui: " << endl;
  switch (GET_MSG(msg)) {

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {

    case kCM_MENU:
      switch (parm1) {

      case M_FILE_OPEN:
	{
	  static TString dir("/pool");
	  TGFileInfo fi;
	  fi.fFileTypes = filetypes;
	  fi.fIniDir    = StrDup(dir);
	  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
	  printf("Open file: %s (dir: %s)\n", fi.fFilename,
		 fi.fIniDir);
	  dir = fi.fIniDir;
	  ppp->open_file(fi.fFilename);
	  file_is_open = true;
	  ppp->get_database();
	}
	break;

      case M_HIST_DRAW:
	{
	  DrawHist();
	}
	break;
		     
      case M_FILE_EXIT:
	if(file_is_open) ppp->close_file();
	CloseWindow();   // this also terminates theApp
	break;

      case M_PMT_1:
	pmt_page = 0;
	current_pmt = 1;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_2:
	pmt_page = 0;
	current_pmt = 2;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_3:
	pmt_page = 0;
	current_pmt = 3;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_4:
	pmt_page = 0;
	current_pmt = 4;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_5:
	pmt_page = 0;
	current_pmt = 5;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_6:
	pmt_page = 0;
	current_pmt = 6;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_7:
	pmt_page = 0;
	current_pmt = 7;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_8:
	pmt_page = 0;
	current_pmt = 8;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_9:
	pmt_page = 0;
	current_pmt = 9;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_10:
	pmt_page = 0;
	current_pmt = 10;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_11:
	pmt_page = 0;
	current_pmt = 11;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_12:
	pmt_page = 0;
	current_pmt = 12;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_13:
	pmt_page = 0;
	current_pmt = 13;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_14:
	pmt_page = 0;
	current_pmt = 14;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_15:
	pmt_page = 0;
	current_pmt = 15;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_16:
	pmt_page = 0;
	current_pmt = 16;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_17:
	pmt_page = 0;
	current_pmt = 17;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_18:
	pmt_page = 0;
	current_pmt = 18;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_19:
	pmt_page = 0;
	current_pmt = 19;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_20:
	pmt_page = 0;
	current_pmt = 20;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_21:
	pmt_page = 0;
	current_pmt = 21;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_22:
	pmt_page = 0;
	current_pmt = 22;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_23:
	pmt_page = 0;
	current_pmt = 23;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_24:
	pmt_page = 0;
	current_pmt = 24;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_25:
	pmt_page = 0;
	current_pmt = 25;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_26:
	pmt_page = 0;
	current_pmt = 26;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_27:
	pmt_page = 0;
	current_pmt = 27;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_28:
	pmt_page = 0;
	current_pmt = 28;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_29:
	pmt_page = 1;
	current_pmt = 29;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_30:
	pmt_page = 0;
	current_pmt = 30;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_31:
	pmt_page = 0;
	current_pmt = 31;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_32:
	pmt_page = 0;
	current_pmt = 32;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_33:
	pmt_page = 0;
	current_pmt = 33;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_34:
	pmt_page = 0;
	current_pmt = 34;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_35:
	pmt_page = 0;
	current_pmt = 35;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_36:
	pmt_page = 0;
	current_pmt = 36;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_37:
	pmt_page = 0;
	current_pmt = 37;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_38:
	pmt_page = 0;
	current_pmt = 38;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_ALL_1:
	pmt_page = 1;
	current_pmt = 0;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_ALL_2:
	pmt_page = 2;
	current_pmt = 0;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_ALL_3:
	pmt_page = 3;
	current_pmt = 0;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_ALL_4:
	pmt_page = 4;
	current_pmt = 0;
	if(file_is_open) this->DrawHist();
	break;
      case M_PMT_ALL_5:
	pmt_page = 5;
	current_pmt = 0;
	if(file_is_open) this->DrawHist();
	break;

      default:
	break;
      }

    case kCM_BUTTON:
      printf("Button was pressed, id = %ld\n", parm1);
      if(parm1 == 154) {
	int run_to_read, ev_to_read; TString rrlabel, evlabel;
	run_to_read = (int) fRun->GetNumber();
	ev_to_read = (int) fEvent->GetNumber();
 	if( (run_to_read != 0) && (ev_to_read != 0) ) {                        
 	  cout << "run = " << run_to_read << " event = " << ev_to_read << endl;
 	  ppp->get_event(run_to_read, ev_to_read);
 	} else {
 	  ppp->readevent();
 	}
	fRun->SetNumber(ppp->nRUN);
	fEvent->SetNumber(ppp->nEVENT+1);
	ppp->process_event();
	this->DrawHist();
	rrlabel+="Run#   "; rrlabel+=ppp->nRUN;
	evlabel+="Event# "; evlabel+=ppp->nEVENT;
	fLabelRun->SetText(rrlabel);
	fLabelEvent->SetText(evlabel);
      }
      break;

    case kCM_CHECKBUTTON:
      printf("CheckButton was pressed, id = %ld\n", parm1);
      if(parm1 == 155) {
	if(fInte->GetState() == kButtonDown) {
	  show_integ_win=true;
	} else {
	  show_integ_win=false;
	}
	if(file_is_open) this->DrawHist();
      }
      break;

    default:
      break;
    }
	 
  default:
    break;
  }
   
  return kTRUE;
}

//  extern float adcc[];
// extern float filt[];
// extern  fftr[];
// extern float ffti[];

int ret;
double hmax;
int nbin;

Double_t ser(Double_t *x, Double_t *par) {

  Double_t xx,offset,peak;

  offset = par[0];
  xx = x[0]-offset;

  peak = 0.0;
  if(xx > 0) peak = par[2]*xx*xx*TMath::Exp(-xx/par[3]);
  peak += par[1];

  cout << x[0] << "  " <<  par[0] << "  " <<  par[1] << "  " <<  par[2] << "  " <<  par[3] << "  " << peak << endl;

  return peak;

}

void WEDMainFrame::DrawHist(void) {

  float *ddd;
  int show_analysis;
  int pmt_start, pmt_end;
	
  show_analysis = 1;
  //  if(ret != 0) show_analysis = 1;
	
  switch(show_analysis) {
  case 1:
    ddd = adcc;
    nbin = ppp->get_nsamples();
    hmax = ((float) nbin)/(1000.0*ppp->get_sampling());
    break;
  case 2:
    ddd = fftspec;
    nbin = (int)((ppp->get_nsamples())/2+1);
    hmax = 1./(2*0.000000001);//frequency	
    break;
  default:
    return;

  }
  int npmt = ppp->nPMT;
  cout << "Found: " << npmt << " PMTs" << endl;
//   npmt = min(npmt,8);

  pmt_start = (pmt_page-1)*8;
  while(pmt_start > npmt) {pmt_page--; pmt_start -= 8;}
  if(pmt_start < 0) pmt_page = 1; pmt_start = 0;
  pmt_end   = min(pmt_page*8, npmt);

  cout << "pmt_start = " << pmt_start << endl;
  cout << "pmt_end   = " << pmt_end   << endl;

  char appPMT1[40];
  char appPMT2[40];
  char appPMT3[40];

  for (int ipmt=0;ipmt<npmt;ipmt++) {
    sprintf(appPMT1,"myh%i",ipmt+1);
    sprintf(appPMT2,"pmt%i",ipmt+1);
    sprintf(appPMT3,"myi%i",ipmt+1);
    if(fHist[ipmt] == NULL) fHist[ipmt] = new TH1F(appPMT1,appPMT2, nbin, 0., hmax);
    if(fHistIntegWin[ipmt] == NULL) fHistIntegWin[ipmt] = new TH1F(appPMT3,appPMT2, nbin, 0., hmax);
  }

  float val; int iii;
  for (int ipmt=0;ipmt<npmt;ipmt++) {
    iii=nbin*ipmt;
    for(int i=0; i<nbin; i++) {
      fHist[ipmt]->SetBinContent(i+1,ddd[i+iii]);
      val = 0.0;
      if(isAboveThreshold[i+iii] == true) val = (float) pmtthreshold[ipmt];
      fHistIntegWin[ipmt]->SetBinContent(i+1, val);
    }
  }

  cout << "Current PMT  = " << current_pmt << endl;
  cout << "Current page = " << pmt_page << endl;
  cout << "Last View    = " << last_view << endl;

  if(current_pmt == 0) {
    if(last_view != 0) {
      fTCanvas1->Clear();
      fTCanvas1->Divide(2,4); //2 columns, 4 rows
    }
    for(int i=pmt_start; i<pmt_end; i++) {
      fTCanvas1->cd((i%8)+1);
      if (fHist[i]) {
	fHist[i]->Draw();
	if(show_integ_win) {
	  fHistIntegWin[i]->SetLineColor(kRed);
	  fHistIntegWin[i]->SetLineWidth(2);
	  fHistIntegWin[i]->Draw("same");
	}
      }
    }
  } else {
    if(last_view == 0) {
      fTCanvas1->Clear();
      fTCanvas1->Divide(1,1);
    }
    fTCanvas1->cd();
    fHist[current_pmt-1]->Draw();
    if(show_integ_win) {
      fHistIntegWin[current_pmt-1]->SetLineColor(kRed);
      fHistIntegWin[current_pmt-1]->SetLineWidth(2);
      fHistIntegWin[current_pmt-1]->Draw("same");
    }
  }

  last_view = current_pmt;

  for(int i=0; i<npmt; i++) {
    fHist[i]->GetXaxis()->SetTitle("Time (#mus)");
    fHist[i]->GetYaxis()->SetTitle("ADC counts");
    fHist[i]->GetXaxis()->SetTitleSize(0.08);
    fHist[i]->GetYaxis()->SetTitleSize(0.08);
    fHist[i]->GetXaxis()->SetLabelSize(0.08);
    fHist[i]->GetYaxis()->SetLabelSize(0.08);
  }

  fTCanvas1->Modified();
  fTCanvas1->Update();

  gSystem->ProcessEvents();  // handle GUI events

  this->DrawAnalysis();

}

Double_t fitfunc(Double_t *x, Double_t *par) {

  Double_t fff,c1,c2;

  if(x[0] < 0.0) return 0;

  //   c1 =      par[3] *par[1]*( TMath::Exp(0.02/par[1])-TMath::Exp(-0.02/par[1]) );
  //   c2 = (1.0-par[3])*par[2]*( TMath::Exp(0.02/par[2])-TMath::Exp(-0.02/par[2]) );
  c1 =      par[3] *( TMath::Exp(0.02/par[1])-TMath::Exp(-0.02/par[1]) );
  c2 = (1.0-par[3])*( TMath::Exp(0.02/par[2])-TMath::Exp(-0.02/par[2]) );
  fff = par[0]*(c1*TMath::Exp(-x[0]/par[1])+c2*TMath::Exp(-x[0]/par[2]));

  return fff;

}

//extern int istate;
extern int ff_nn;

void WEDMainFrame::DrawAnalysis(void) {
  //cout << "In DrawAnalysis" << endl;
  float *ddd;
  double *ddd_double;
  double *ddd1_double;
  char c1[]="ADC counts";
  // 	char c2[]="Photoelectrons";
  char c3[]="Amplitude";
  char *cc;
	
  int show_analysis;
	  
  show_analysis = 6;
  //  if(ret != 0) show_analysis = 1;
	
  switch(show_analysis) {
  case 1:
    ddd = adcc;
    cc = c1;
    nbin = ppp->get_nsamples();
    hmax = ((float) nbin)/(1000.0*ppp->get_sampling());
    break;
  case 2:
    ddd = adccsum;
    cc = c1;
    nbin = ppp->get_nsamples();
    hmax = ((float) nbin)/(1000.0*ppp->get_sampling());
    break;
  case 3:
    ddd = adcc_filt;
    cc = c1;
    nbin = ppp->get_nsamples();
    hmax = ((float) nbin)/(1000.0*ppp->get_sampling());
    break;
  case 4:
    ddd_double = fftr;
    ddd1_double = ffti;
    cc = c3;
    nbin = (int)((ppp->get_nsamples())/2+1);
    hmax = 1./(2*0.000000001);//frequency	
    break;
  case 5:
    ddd = fftspec_filt;
    cc = c3;
    nbin = (int)((ppp->get_nsamples())/2+1);
    hmax = 1./(2*0.000000001);//frequency	
    break;
  case 6:
    cc = c1;
    nbin = ppp->get_nsamples();
    //cout << nbin << endl;
    //cout << ppp->get_sampling() << endl;
    hmax = ((float) nbin)/(1000.0*ppp->get_sampling());
    break;
  default:
    return;
  }
	
  if (show_analysis != 6)
    {
      if(fHistAnalysis[0] == NULL) fHistAnalysis[0] = new TH1F("mya5", "mya5", nbin, 0., hmax);
      if(fHistAnalysis[1] == NULL) fHistAnalysis[1] = new TH1F("mya6", "mya6", nbin, 0., hmax);
      if(fHistAnalysis[2] == NULL) fHistAnalysis[2] = new TH1F("mya7", "mya7", nbin, 0., hmax);
      if(fHistAnalysis[3] == NULL) fHistAnalysis[3] = new TH1F("mya8", "mya8", nbin, 0., hmax);
    }

  if(show_analysis<4){
		
    for(int i=0; i<nbin; i++) {
      fHistAnalysis[0]->SetBinContent(i+1,ddd[i+idx[0]]);
      fHistAnalysis[1]->SetBinContent(i+1,ddd[i+idx[1]]);
      fHistAnalysis[2]->SetBinContent(i+1,ddd[i+idx[2]]);
      fHistAnalysis[3]->SetBinContent(i+1,ddd[i+idx[3]]);
    }

  }
	
  if(show_analysis==4){
		
    for(int i=0; i<nbin; i++) {
      //			fHistAnalysis[0]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i],2)+TMath::Power(ddd1_double[i],2)));
      //			fHistAnalysis[1]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin],2)+TMath::Power(ddd1_double[i+nbin],2)));
      //			fHistAnalysis[2]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin*2],2)+TMath::Power(ddd1_double[i+nbin*2],2)));
      //			fHistAnalysis[3]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin*3],2)+TMath::Power(ddd1_double[i+nbin*3],2)));
      fHistAnalysis[0]->SetBinContent(i+1,ave_freq_spect[i]);
      fHistAnalysis[1]->SetBinContent(i+1,ave_freq_spect[i+nbin]);
      fHistAnalysis[2]->SetBinContent(i+1,ave_freq_spect[i+nbin*2]);
      fHistAnalysis[3]->SetBinContent(i+1,ave_freq_spect[i+nbin*3]);

    }
  }
	
  if(show_analysis==5){
	  
    for(int i=0; i<nbin; i++) {
      //			fHistAnalysis[0]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i],2)+TMath::Power(ddd1_double[i],2)));
      //			fHistAnalysis[1]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin],2)+TMath::Power(ddd1_double[i+nbin],2)));
      //			fHistAnalysis[2]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin*2],2)+TMath::Power(ddd1_double[i+nbin*2],2)));
      //			fHistAnalysis[3]->SetBinContent(i+1,TMath::Sqrt(TMath::Power(ddd_double[i+nbin*3],2)+TMath::Power(ddd1_double[i+nbin*3],2)));
      fHistAnalysis[0]->SetBinContent(i+1,fftspec_filt[i]);
      fHistAnalysis[1]->SetBinContent(i+1,fftspec_filt[i+nbin]);
      fHistAnalysis[2]->SetBinContent(i+1,fftspec_filt[i+nbin*2]);
      fHistAnalysis[3]->SetBinContent(i+1,fftspec_filt[i+nbin*3]);
	    
    }
  }
	
  if (show_analysis == 6){
    if(fHistAnalysis[0] == NULL) fHistAnalysis[0] = new TH1F("mya5", "mya5", nbin, 0., hmax);
    int nbin2 = (int) (nbin*ppp->get_sampling()/reducedSamplingFrequency);
    if(fHistAnalysis[1] == NULL) fHistAnalysis[1] = new TH1F("mya6", "mya6", nbin2, 0., hmax);

	
    if(fHistAnalysis[2] == NULL) fHistAnalysis[2] = new TH1F("mya7", "mya7", 151, -0.04, 6.0);
    if(myf==NULL) {
      myf = new TF1("myf", fitfunc, 0.0, 6.0, 4);
      (fHistAnalysis[2]->GetListOfFunctions())->Add(myf);
    }

    if(fHistAnalysis[3] == NULL) fHistAnalysis[3] = new TH1F("mya8", "mya8", nbin, 0., hmax);

    float integral = 0;
    for(int i=0; i<nbin; i++) {
      fHistAnalysis[0]->SetBinContent(i+1,adccsum[i]);
      integral += adccsum[i];
      fHistAnalysis[3]->SetBinContent(i+1,integral);
    }
    for (int i=0;i<nbin2;i++)
      fHistAnalysis[1]->SetBinContent(i+1,adccsumdown[i]);
    //for (int i=0;i<pulse.size();i++)
    //  fHistAnalysis[2]->SetBinContent(i+1,pulse[i]);
    // for (int i=0;i<differential.size();i++)
    //  fHistAnalysis[3]->SetBinContent(i+1,differential[i]);
  }
  /*
  float sum; int iii=1;
  int npmt = ppp->nPMT; if(npmt==8) npmt--;
  for(int i=the_event.t1-40; i<the_event.t1+6000; i+=40) {
    sum = 0.0;
    for(int ipmt=0; ipmt<npmt; ipmt++) {
      for(int j=0; j<40; j++) sum+=adcc[i+j+idx[ipmt]];
    }
    fHistAnalysis[2]->SetBinContent(iii++,sum); 
  }

  sum = 0.0;
  for(int i=1; i<=ff_nn; i++) sum += fHistAnalysis[2]->GetBinContent(i);
  //cout << " +++++ fitfunc: Sum in fit region = " << sum << " S1 = " << the_event.s1 << endl;
  */
  
  for(int i=0; i<4; i++) {
    fTCanvas2->cd(i+1);
//     if( (i==2) && (istate >= 0) ) {
//       myf->SetParameter(0, the_event.fit[0]);
//       myf->SetParameter(1, the_event.fit[1]);
//       myf->SetParameter(2, the_event.fit[2]);
//       myf->SetParameter(3, the_event.fit[3]);
//       cout << " +++++ fitfunc:  @20 ns = " << myf->Eval(0.020) << "  @60 ns = " << myf->Eval(0.060) << "  @100 ns = " << myf->Eval(0.100) << "  @140 ns = " << myf->Eval(0.140) << endl;
//     }
    //if(i==2) { gPad->SetLogy(1); } else { gPad->SetLogy(0); }
    if (i != 2)
      fHistAnalysis[i]->Draw();
  }
	
  for(int i=0; i<4; i++) {
		
    if(show_analysis<4) fHistAnalysis[i]->GetXaxis()->SetTitle("Time (#mus)");
    if(show_analysis==4) fHistAnalysis[i]->GetXaxis()->SetTitle("Frequency (Hz)");
    fHistAnalysis[i]->GetYaxis()->SetTitle(cc);
    fHistAnalysis[i]->GetXaxis()->SetTitleSize(0.08);
    fHistAnalysis[i]->GetYaxis()->SetTitleSize(0.08);
    fHistAnalysis[i]->GetXaxis()->SetLabelSize(0.08);
    fHistAnalysis[i]->GetYaxis()->SetLabelSize(0.08);
  }
	
  fTCanvas2->Modified();
  fTCanvas2->Update();
	
  gSystem->ProcessEvents();  // handle GUI events
}
