
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGMenu.h"
#include "TGFileDialog.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "wep.h"

class WEDMainFrame : public TGMainFrame {
  
private:
/*   TGCompositeFrame   *fMenu;  */
  TGVerticalFrame    *fTop;
  TGHorizontalFrame  *fMain;
  TGVerticalFrame    *fDisplay;
  TGVerticalFrame    *fInfo;
  TGLabel            *fLabelRun;
  TGLabel            *fLabelEvent;
  TGPopupMenu        *fMenuFile;
  TGPopupMenu        *fMenuPMT1;
  TGPopupMenu        *fMenuPMT2;
  TGPopupMenu        *fMenuPMT3;
  TGPopupMenu        *fMenuPMT4;
  TGPopupMenu        *fMenuPMT5;
  TGMenuBar          *fMenuBar;
  TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fCanvasLayout;
  TRootEmbeddedCanvas *fCanvas1;
  TRootEmbeddedCanvas *fCanvas2;

  TGButton           *fNext;
  TGCheckButton      *fInte;
  TGNumberEntry      *fRun;
  TGNumberEntry      *fEvent;
  TCanvas            *fTCanvas1;
  TCanvas            *fTCanvas2;
  TH1F               *fHist[40];
  TH1F               *fHistIntegWin[40];
  TH1F               *fHistAnalysis[4];
  wep                *ppp;
  TF1                *myf;
  bool                file_is_open, show_integ_win;
  int                 current_pmt;
  int                 last_view;
  int                 pmt_page;
public:
  WEDMainFrame(wep *ptr, const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~WEDMainFrame();

  void DrawHist(void);
  void DrawAnalysis(void);

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
};

