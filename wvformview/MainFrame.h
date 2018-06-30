#ifndef __MY_MAINFRAME__
#define __MY_MAINFRAME__


#include <TGTab.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include "PMTwidget.h"


#define NPMTS 38




class MainFrame:public TGMainFrame
{


public:
MainFrame(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options, int NCHANNELS);
~MainFrame();


void draw_histo_in_pad(TObject * histo,int pad,TString options=" ");
void draw_histo_in_pad(int pad);  //clear pad since histo present
//void draw_second_histo_in_pad(TObject * histo,int pad);
void draw_sumhisto_in_pad(TObject * histo,int pad);
void draw_second_sumhisto_in_pad(TObject * histo2,int pad);
//void set_all_status(int status);
PMTwidget * get_display_widget(int number);
TRootEmbeddedCanvas * get_sumdisplay_canvas(int number);

TGTextButton * get_abort_button();
TGTextButton * get_next_button();
TGTextButton * get_prev_button();

TGTextButton * get_event_button();

TGTextButton * get_field_button(int button);

 TGCheckButton *get_integ_button();
 TGCheckButton *get_raw_button();
 
 void set_channel_label(int pad,TString text);
 
int get_run_number();
int get_event_number();

private:

   int fNbuttons;

   TGTab *fTab;
   TGCompositeFrame *fCompositeGroup[6];
   //TGCompositeFrame *fCompositeGroup2;
   //TGCompositeFrame *fCompositeGroup3;
   //TGCompositeFrame *fCompositeGroup4;
   TGTextButton *fAbortButton;
   TGTextButton *fNextButton;
   TGTextButton *fPrevButton;
   TGTextButton *fEventButton;
   TGTextButton *fGroupButton[8]; 
   
   TGCheckButton *fIntegButton;
   
   TGCheckButton *fRawButton;
   
   
   TGNumberEntryField * fRunEntry; 
   TGNumberEntryField * fEvtEntry; 
   
   PMTwidget * displays[NPMTS];

   TRootEmbeddedCanvas * sumdisplays[2];
	TCanvas * canvas[2];
   
ClassDef(MainFrame,1);
};

#endif
