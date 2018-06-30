// Mainframe macro generated from application: /usr/bin/root.exe
// By ROOT version 5.17/05 on 2009-05-09 10:55:41


#include "PMTwidget.h"
#include <iostream>
//PMTwidget::PMTwidget(TGClient* c, Window_t id, const TGWindow* parent=0):TGCompositeFrame(c,id,parent)
//(const TGWindow* p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())


ClassImp(PMTwidget)


PMTwidget::PMTwidget(const TGWindow* p, UInt_t w , UInt_t h, UInt_t options , Pixel_t back):TGCompositeFrame(p,w,h,options,back)
{

   // main frame

  // SetLayoutBroken(kTRUE);
   fPMTLabel = new TGLabel(this,"PMT XX");
   fPMTLabel->SetTextJustify(36);
   fPMTLabel->SetMargins(0,0,0,0);
   fPMTLabel->SetWrapLength(-1);
   this->AddFrame(fPMTLabel, new TGLayoutHints(kLHintsCenterX | kLHintsTop,2,2,2,2));
   fPMTLabel->MoveResize(136,8,176,40);
   // embedded canvas
   fEmbeddedCanvas = new TRootEmbeddedCanvas(0,this,440,192);
   Int_t wfEmbeddedCanvas = fEmbeddedCanvas->GetCanvasWindowId();
   canvas = new TCanvas("canvas", 10, 10, wfEmbeddedCanvas);
   fEmbeddedCanvas->AdoptCanvas(canvas);
   this->AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX |kLHintsExpandY,2,2,2,2));
   fEmbeddedCanvas->MoveResize(32,64,440,192);

   batch_refit=false;

//   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
//    GCValues_t vall664;
//     ULong_t ucolor; 
//          // will reflect user GC changes graphics context changes
//    
//   vall664.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
//    gClient->GetColorByName("#ffffff",vall664.fForeground);
//    gClient->GetColorByName("#c0c0c0",vall664.fBackground);
//    vall664.fFillStyle = kFillSolid;
//    vall664.fFont = ufont->GetFontHandle();
//    vall664.fGraphicsExposures = kFALSE;
//    uGC = gClient->GetGC(&vall664, kTRUE);

        // will reflect user color changes
 fStatusLabel = new TGLabel(this,"Status: YY");
    
 //fStatusLabel = new TGLabel(this,"Status: YY",uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);
   fStatusLabel->SetBackgroundColor(0xc0c0c0);
     fStatusLabel->SetForegroundColor(0xFFFFFF);
   fStatusLabel->SetTextJustify(36);
   fStatusLabel->SetMargins(0,0,0,0);
   fStatusLabel->SetWrapLength(-1);
   this->AddFrame(fStatusLabel, new TGLayoutHints(kLHintsCenterX |kLHintsExpandX | kLHintsTop,2,2,2,2));
   fStatusLabel->MoveResize(128,272,208,40);

    channel_status=0;

TGHorizontalFrame *hframemain= new TGHorizontalFrame(this,300,300);


   fbatchButton = new TGCheckButton(hframemain,"turn PMT off");
   fbatchButton->SetToolTipText("mark for turning the PMT off in the DB");
   fbatchButton->SetTextJustify(36);
   fbatchButton->SetMargins(0,0,0,0);
   fbatchButton->SetWrapLength(-1);
   hframemain->AddFrame(fbatchButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fbatchButton->MoveResize(48,336,128,32);



   fRefitTextButton = new TGTextButton(hframemain,"Refit this PMT");
   fRefitTextButton->SetTextJustify(36);
   fRefitTextButton->SetMargins(0,0,0,0);
   fRefitTextButton->SetWrapLength(-1);
   fRefitTextButton->Resize(192,48);
   fRefitTextButton->SetToolTipText("Refit PMT setting parameters by hand");
   hframemain->AddFrame(fRefitTextButton, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   fRefitTextButton->MoveResize(288,328,192,48);




this->AddFrame(hframemain, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
   //this->SetMWMHints(kMWMDecorAll,
   //                     kMWMFuncAll,
   //                     kMWMInputModeless);
   this->MapSubwindows();

   this->Resize(this->GetDefaultSize());
   this->MapWindow();
   this->Resize(w,h);
}  


PMTwidget::~PMTwidget()
{
//   delete fEmbeddedCanvas;
//    delete canvas;
//    delete fPMTLabel;
//     delete ufont;         // will reflect user font changes
//    TGGC   *uGC;           // will reflect user GC changes
//    TGLabel *fStatusLabel;
//    TGTextButton *fRefitTextButton;
//    TGCheckButton *fbatchButton;



std::cout << "closing widget" << std::endl;



}


int PMTwidget::get_channel_status()
{
return channel_status;
}


void PMTwidget::set_channel_status(int status)
{

channel_status=status;

if(status==0)
  {fStatusLabel->SetBackgroundColor(0x0000FF );
fStatusLabel->SetForegroundColor(0xFFFFFF  );
   fStatusLabel->SetText("status: pending");
  }

if(status==1)
  {fStatusLabel->SetBackgroundColor(0x00FF00  );
   fStatusLabel->SetForegroundColor(0x000000  );
   fStatusLabel->SetText("status: converged");
  }

if(status==2)
  {
  // gClient->GetColorByName("#ffffff",vall664.fForeground);
  // gClient->GetColorByName("#777777",vall664.fBackground);
   fStatusLabel->ChangeBackground(0xFF0000);
      fStatusLabel->SetForegroundColor(0xFFFFFF  );
   fStatusLabel->SetText("status: not converged");
  }

   this->MapSubwindows();
   this->MapWindow();

}


int PMTwidget::get_channel_number()
{
return channel_number;
}


void PMTwidget::set_channel_number(int number)
{

channel_number=number;
fPMTLabel->SetText(Form( "Channel nr %d",number ));

}


void PMTwidget::toggle_batch_refit()
{
bool refit=fbatchButton->IsOn();
std::cout << " setting refit to " << refit << std::endl;
batch_refit=refit;
}

bool PMTwidget::get_batch_refit()
{
return batch_refit;
}



TCanvas * PMTwidget::get_canvas()
{

return fEmbeddedCanvas->GetCanvas();

}


TGTextButton * PMTwidget::get_refit_button()
{
return fRefitTextButton;;
}

TGCheckButton * PMTwidget::get_check_button()
{
return fbatchButton;
}

