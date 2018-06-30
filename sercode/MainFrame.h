#ifndef __MY_MAINFRAME__
#define __MY_MAINFRAME__


#include <TGTab.h>
#include <TGFrame.h>
#include <TGButton.h>

#include "PMTwidget.h"


#define NPMTS 38




class MainFrame:public TGMainFrame
{


public:
MainFrame(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options,int run);
~MainFrame();


void draw_histo_in_pad(TH1F * histo,int pad);
void set_all_status(int status);
PMTwidget * get_display_widget(int number);
void GetOffWindows(int *onoff,int nchans);

TGTextButton * get_abort_button();
TGTextButton * get_saveDB_button();
TGTextButton * get_savefile_button();
TGTextButton * get_batch_button();


TGTextButton * get_ser_button();
TGTextButton * get_amp_button();
TGTextButton * get_length_button();

private:

   TGTab *fTab;
   TGCompositeFrame *fCompositeGroup[5];
   //TGCompositeFrame *fCompositeGroup2;
   //TGCompositeFrame *fCompositeGroup3;
   //TGCompositeFrame *fCompositeGroup4;
   TGTextButton *fAbortButton;
   TGTextButton *fSaveDBButton;
   TGTextButton *fBatchButton;
   TGTextButton *fSaveFileButton;
   
   
   TGTextButton *fserButton;
   TGTextButton *fampButton;
   TGTextButton *flengthButton;		
		
   PMTwidget * displays[NPMTS];

   int runin;
   

ClassDef(MainFrame,1);
};

#endif
