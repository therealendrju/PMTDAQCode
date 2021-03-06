#ifndef __MY_AUXFRAME__
#define __MY_AUXFRAME__


#include <TGTab.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TH1F.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TCanvas.h>

class AuxFrame:public TGMainFrame
{


public:
AuxFrame(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options);
~AuxFrame();


void draw_histo_in_pads(TObject * histo,int pad,TString option);
//void draw_second_histo_in_pad(TObject * histo,int pad);
//TGTextButton * get_refit_button();
TGTextButton * get_done_button();

//void set_params_in_frame(double * pval,double *plow,double *phigh,int ngaus);
//void get_params_from_frame(double * pval,double *plow,double *phigh);

private:

  // int NEXP;
  // int NGAUSS;

  //void SetupRightAuxiliaryFrame( TGVerticalFrame *  vframeright);
 // void Expand(TGCompositeFrame *f3,TString label,Int_t i);
  //void draw_extra_func_in_pads(TH1F * histo,int pad);
  
   TRootEmbeddedCanvas * fEcanvasup;
   TRootEmbeddedCanvas * fEcanvasd;   
 //  TGTextButton *draw;  
   TGTextButton *done;

//  TGNumberEntryField *fGauss;

// TGNumberEntryField *fHigh[1+2*2+2+5]; //   baseline, 2 exponentials, xo, sigma, 5 gaussian peaks.  
 // TGNumberEntryField *fLow[1+2*2+2+5];
// TGNumberEntryField *fVal[1+2*2+2+5];
//TGCompositeFrame *fCompositeGroup2;
   //TGCompositeFrame *fCompositeGroup3;
   //TGCompositeFrame *fCompositeGroup4;
//    TGTextButton *fAbortButton;
//    TGTextButton *fSaveDBButton;
//    TGTextButton *fBatchButton;



ClassDef(AuxFrame,1);

};

#endif
