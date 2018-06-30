
#ifndef __MY_PMTwidget__
#define __MY_PMTwidget__

#include <TH1F.h>
//#include <TGCompositeFrame.h>
#include <TGFrame.h>

#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif

#include <iostream>

enum {pending,converged,notconverged};


class PMTwidget:public TGCompositeFrame
{

public:

//PMTwidget(TGClient* c, Window_t id, const TGWindow* parent);
PMTwidget(const TGWindow* p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground());
~PMTwidget();

//void draw_histo(TH1F * inhist);

//void set_channel_status(int status);
//int get_channel_status();

void set_channel_number(int number);
int get_channel_number();

void set_channel_label(TString text);

//void toggle_batch_refit();
//bool get_batch_refit();

TGTextButton * get_closeup_button();
//TGCheckButton * get_check_button();

TCanvas * get_canvas();

private:

 //  TGMainFrame *fMainFrame1148;
   TRootEmbeddedCanvas *fEmbeddedCanvas;
   TCanvas *canvas;
   TGLabel *fPMTLabel;
 /*  TGFont *ufont;         // will reflect user font changes
   TGGC   *uGC;           // will reflect user GC changes
   */ // TGLabel *fStatusLabel;
   TGTextButton *fCloseupTextButton;
  // TGCheckButton *fbatchButton;
 
         // will reflect user font changes	



//   int channel_status;
   int channel_number;
  // bool batch_refit;


ClassDef(PMTwidget,1);

};


#endif

