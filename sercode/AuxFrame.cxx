#include "AuxFrame.h"
#include <iostream>
#include <TF1.h>
#include <TMath.h>

ClassImp(AuxFrame)


AuxFrame::AuxFrame(const TGWindow *p, UInt_t w, UInt_t h,UInt_t options, int nexp=2,int ngaus=2 ) : TGMainFrame(p,w,h,options),NEXP(nexp),NGAUSS(ngaus)
{
 TGHorizontalFrame *hframemain=new TGHorizontalFrame(this,200,200);

  //left side ----------------------------------------
  TGVerticalFrame *vframeleft=new TGVerticalFrame(hframemain,350,250);

  fEcanvasup = new TRootEmbeddedCanvas("Ecanvas1",vframeleft,350,250);
  vframeleft-> AddFrame(fEcanvasup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));
  
  fEcanvasd = new TRootEmbeddedCanvas("Ecanvas2",vframeleft,350,250);
  vframeleft-> AddFrame(fEcanvasd, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));
  
  hframemain->AddFrame(vframeleft,new TGLayoutHints(kLHintsCenterX|kLHintsExpandY|kLHintsExpandX,2,2,2,2) );

  // end of left side!!!
  //------------------------------------------------

  //Right side --------------------------------------
  
  vframeright=new TGVerticalFrame(hframemain,200,200);

  SetupRightAuxiliaryFrame(vframeright);

  hframemain->AddFrame(vframeright,new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  // end of right side -------------------------------------
  
  AddFrame(hframemain,new TGLayoutHints(kLHintsCenterX|kLHintsExpandY|kLHintsExpandX,2,2,2,2) );
  
  TGHorizontalFrame *hframe= new TGHorizontalFrame(this,200,40);
  
  /////// buttons
  //char temp[10];
  //sprintf(temp,"doFit(=%d)",pad);
  draw = new TGTextButton(hframe,"&Fit Again");
  //draw->Connect("Clicked()","MyMainFrame",this,temp);
  hframe->AddFrame(draw,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  
  
  done = new TGTextButton(hframe,"&done ");
 // done->Connect("Clicked()","MyMainFrame",this,"Close()");
  hframe->AddFrame(done, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  
  AddFrame(hframe,new TGLayoutHints(kLHintsCenterX,2,2,2,2) );
  
  
  // sets window name and shows main frame 
  SetWindowName("Auxiliary Window");
  MapSubwindows();
  Resize(this->GetDefaultSize());
  MapWindow();

}  



void AuxFrame::SetupRightAuxiliaryFrame( TGVerticalFrame *  vframeright)
{  
  TGLabel *fRadiusLabel = new TGLabel(vframeright, "FITTING PARAMETERS");
  vframeright->AddFrame(fRadiusLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 5, 5));

  //TGCompositeFrame * f3;
  //f3=new TGCompositeFrame(vframeright, 200,30, kHorizontalFrame);
  
//   fRadiusLabel = new TGLabel(f3, "Number of Exp functions");
//   f3->AddFrame(fRadiusLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
//   
// 
//   /*---- number of Exponential functions ----*/
//   fExp = new TGNumberEntryField(f3, 1, Event::NEXP, 
//                                     TGNumberFormat::kNESInteger,
//                                     TGNumberFormat::kNEANonNegative, 
//                                     TGNumberFormat::kNELNoLimits);
//   fExp->SetToolTipText("Enter number of Exp functions to be fitted");
//   f3->AddFrame(fExp, new TGLayoutHints(kLHintsRight, 18, 1, 1, 1));
// 
//   vframeright->AddFrame(f3,
// 			new TGLayoutHints(kLHintsRight|kLHintsCenterY,5,5,5,5));
//   fExp->Connect("ReturnPressed()","MyMainFrame",this,"SetNFuncs(=1)");


  const Int_t PARS=1+NEXP*2+2+NGAUSS;
  TGCompositeFrame *f3[PARS];

  /*---- number of Gaussian functions ----*/
   f3[0]=new TGCompositeFrame(vframeright, 200,30, kHorizontalFrame);
//   
  fRadiusLabel = new TGLabel(f3[0], "Number of Gauss functions");
  f3[0]->AddFrame(fRadiusLabel, 
	       new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));

  /*---- low boundary ----*/
  fGauss = new TGNumberEntryField(f3[0], 1, NGAUSS, 
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
  
  fGauss->SetToolTipText("Enter number of Gauss functions to be fitted");
  f3[0]->AddFrame(fGauss, new TGLayoutHints(kLHintsCenterX, 18, 1, 1, 1));

  change = new TGTextButton(f3[0],"&Change");
  //draw->Connect("Clicked()","MyMainFrame",this,temp);
  f3[0]->AddFrame(change,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  
  
  
  
  vframeright->AddFrame(f3[0],
			new TGLayoutHints(kLHintsRight|kLHintsCenterY,5,5,5,5)); 
//   fGauss->Connect("ReturnPressed()","MyMainFrame",this,"SetNFuncs(=2)");

  //////// end of Gauss field
  

  
  //setting the parameter names...

  int Amp=0;
  for(Int_t i=0;i<PARS;i++)
    {
      char  temp[15];
      if(i==0)
	sprintf(temp,"const noise A");
      else if(i>0 && i<=2*NEXP)
	{
	  if(i%2)
	    sprintf(temp,"B%d",(i-1)/2+1);
	  else
	    sprintf(temp,"T%d",(i-1)/2+1);
	}
      else if(i==(2*NEXP+1))
	sprintf(temp,"Xo");
      else if(i==(2*NEXP+2))
	sprintf(temp,"sigma");
      else 
	sprintf(temp,"P%d",Amp++);
   //   cout << temp<< endl;      
      // creating the actual entry fields:
      //      f3[i]=new TGCompositeFrame[PARS](vframeright, 200,30, kHorizontalFrame); 
      f3[i]=new TGCompositeFrame(vframeright, 200,30, kHorizontalFrame); 
      Expand(f3[i],temp,i);

      vframeright->AddFrame(f3[i], new TGLayoutHints(kLHintsRight,2,2,2,2));
    }
}





void AuxFrame::Expand(TGCompositeFrame *f3,TString label,Int_t i)
{
  TString *temp;

  TGLabel *fRadiusLabel = new TGLabel(f3, label);
  f3->AddFrame(fRadiusLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));

  /* ----- high boundary -----*/
  //   fHigh[i] = new TGNumberEntryField(f3, 1, p[1][i],
  //                                     TGNumberFormat::kNESRealOne,
  //                                     TGNumberFormat::kNEANonNegative,
  //                                     TGNumberFormat::kNELNoLimits);
  fHigh[i] = new TGNumberEntryField(f3, 1, 0,
                                    TGNumberFormat::kNESReal,
                                    TGNumberFormat::kNEAAnyNumber,
                                    TGNumberFormat::kNELNoLimits);
 
fHigh[i]->SetNumber(i);
     std::cout << "creating fhigh " << i << " " << fHigh[i]->GetNumber() << std::endl;

  temp=new TString("High boundary of parameter ");
  temp->Append(label);
  fHigh[i]->SetToolTipText(temp->Data());
  f3->AddFrame(fHigh[i], new TGLayoutHints(kLHintsRight, 18, 1, 1, 1));

  delete temp;

 /*---- low boundary ----*/

//  fLow[i] = new TGNumberEntryField(f3, 1, p[0][i],
//                                     TGNumberFormat::kNESRealOne,
//                                     TGNumberFormat::kNEANonNegative,
//                                     TGNumberFormat::kNELNoLimits);
  fLow[i] = new TGNumberEntryField(f3, 1, 0,
                                   TGNumberFormat::kNESReal,
                                   TGNumberFormat::kNEAAnyNumber,
                                   TGNumberFormat::kNELNoLimits);

  temp=new TString("Low boundary of parameter ");
  temp->Append(label);
  fLow[i]->SetToolTipText(temp->Data());
  f3->AddFrame(fLow[i], new TGLayoutHints(kLHintsRight, 18, 1, 1, 1));
  delete temp;



  /* ------- value -----------*/
  // fVal[i] = new TGNumberEntryField(f3, 1, p[2][i],
  //                                  TGNumberFormat::kNESRealOne,
  //                                  TGNumberFormat::kNEANonNegative,
  //                                  TGNumberFormat::kNELNoLimits);
  fVal[i] = new TGNumberEntryField(f3, 1, 0,
                                   TGNumberFormat::kNESReal,
                                   TGNumberFormat::kNEAAnyNumber,
                                   TGNumberFormat::kNELNoLimits);
 // fVal[i]->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0);


  temp=new TString("Value of parameter ");
  temp->Append(label);
  fVal[i]->SetToolTipText(temp->Data());
  f3->AddFrame(fVal[i], new TGLayoutHints(kLHintsRight, 18, 1, 1, 1));

  delete temp;
  /* --------- connecting the signals -----------*/
//   char *name=new char[40];
//   sprintf(name,"SetValue(=%d)",i*3);
//   fLow[i]->Connect("ReturnPressed()","MyMainFrame",this,name);
//   sprintf(name,"SetValue(=%d)",i*3+1);
//   fHigh[i]->Connect("ReturnPressed()","MyMainFrame",this,name);
//   sprintf(name,"SetValue(=%d)",i*3+2);
//   fVal[i]->Connect("ReturnPressed()","MyMainFrame",this,name);

}



AuxFrame::~AuxFrame()
{

this->UnmapWindow();


}




void AuxFrame::set_params_in_frame(double * pval,double *plow,double *phigh,int ng)
{
NGAUSS=ng;

const Int_t NPARS=1+NEXP*2+2+NGAUSS;

for(int i=0;i<NPARS;i++)
	{
	
std::cout << "@@@ " << phigh[i] << " " << plow[i] << " " << pval[i] << " "  <<std::endl;
	fHigh[i]->SetNumber(phigh[i]);
	fLow[i]->SetNumber(plow[i]);
	fVal[i]->SetNumber(pval[i]); 

	}

}



void AuxFrame::get_params_from_frame(double * pval,double *plow,double *phigh)
{

  const Int_t NPARS=1+NEXP*2+2+NGAUSS;

for(int i=0;i<NPARS;i++)
	{
	
	phigh[i]=fHigh[i]->GetNumber();
	plow[i]=fLow[i]->GetNumber();
	pval[i]=fVal[i]->GetNumber(); 
 	}

}



void AuxFrame::draw_histo_in_pads(TH1F * histo,int pad,int mode)
{

   fEcanvasup->GetCanvas()->cd();

     histo->DrawCopy();	
     if(!mode)
     draw_extra_func_in_pads(histo,pad);
     
   
   fEcanvasd->GetCanvas()->cd();   
   fEcanvasd->GetCanvas()->SetLogy();

     histo->DrawCopy();	
     if(!mode)
     draw_extra_func_in_pads(histo,pad);
     
     
     fEcanvasd->GetCanvas()->Update();
     fEcanvasup->GetCanvas()->Update();
}



void AuxFrame::draw_extra_func_in_pads(TH1F * histo,int pad)
{
  Double_t Xo,sigma;
  
  histo->GetXaxis()->SetTitle("channel");
  histo->GetYaxis()->SetTitle("counts");
  
  /* ---- set Axis ------*/
   TString funcname=Form("func nr%d",pad);
   TF1 *func = (TF1*)histo->GetFunction(funcname);
   Int_t ParXo=NEXP*2+1;    // number of Xo param.
   Int_t ParSig=NEXP*2+2;   // number of sigma param
  
   Double_t Xran;
  for(Double_t x=func->GetParameter(ParXo);func->Eval(x,0,0,0)>=.1+func->GetParameter(0) && x<=8000 ;x+=func->GetParameter(ParSig))	
    {
      Xran=x;
     }
  if(Xran>=5200) Xran=5200;
  
  ///////////////////////////
  Xo=func->GetParameter(ParXo);
  sigma=func->GetParameter(ParSig);
 
 // myhist->SetLineColor(2);
  histo->SetAxisRange(0,Xran,"X");
  
 // hist->Draw();
  
  TF1 * g1=new TF1("g1","gaus",5,8000);
  TF1 * e1=new TF1("e1","expo",5,8000);
  
 // TF1 * tt=new TF1("tt","expo",5,8000);
  
  e1->SetLineColor(kBlue);
  e1->SetLineWidth(1);
  
  g1->SetLineColor(kBlue);
  g1->SetLineWidth(1);
  
  //expo loop
  for(Int_t i=0;i<2*NEXP;i+=2)
    {
      Double_t p0=func->GetParameter(i+1);
      e1->SetParameters(p0,func->GetParameter(i+2));
      e1->DrawCopy("LSAME");
    }
  
  //Gauss loop
  for(Int_t i=0;i<NGAUSS;i++)
    {
      g1->SetParameters(func->GetParameter(i+NEXP*2+3),(i+1)*Xo,TMath::Sqrt((double)(i+1))*sigma);
      g1->DrawCopy("LSAME");
    }
  
  delete g1;
  delete e1;





}


// int AuxFrame::ClearRightAuxiliaryFrame(TGVerticalFrame *vframeright)
// {
// 	
// 	const Int_t PARS=1+NEXP*2+2+NGAUSS;
// 	for(int i=0;i<PARS;i++)
// 	{
// 		delete fVal[i]; 
// 		delete fLow[i];
// 		delete fHigh[i];
// 		}	
// 	}
		


int AuxFrame::redraw_frame_with_gauss(int ngauss)
{

	NGAUSS=ngauss;
	
	TGHorizontalFrame *hframemain=(TGHorizontalFrame *)(vframeright->GetParent()); 
 //  hframemain->UnmapWindow();
//   vframeright->UnmapWindow();
 
	this->HideFrame(hframemain);
	std::cout << "before cleanup" << std::endl;
  // vframeright->DestroySubwindows();
//   hframemain->RemoveFrame(vframeright);
 
	delete vframeright;
  
	vframeright=new TGVerticalFrame(hframemain,200,200);
	std::cout << "after cleanup" << std::endl;
  
	SetupRightAuxiliaryFrame(vframeright);
//   hframemain->AddFrame(vframeright,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
	this->ShowFrame(hframemain);
	this->MapSubwindows();
	this->Resize(this->GetDefaultSize());
	this->MapWindow();
}	



TGTextButton * AuxFrame::get_refit_button()
{
return draw;  
   
}

TGTextButton * AuxFrame::get_change_button()
{
	return change;  
   
}


TGTextButton * AuxFrame::get_done_button()
{
return done;
}


int AuxFrame::get_gauss_number()
{
	
	return fGauss->GetIntNumber();
		
	}

