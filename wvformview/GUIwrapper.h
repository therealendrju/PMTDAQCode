#ifndef __MY_GUI_WRAPPER_
#define __MY_GUI_WRAPPER_


#include "AuxFrame.h"
#include "MainFrame.h"
#include <iostream>
#include <TApplication.h>
#ifndef _CINT_ 
#include "process.h"
#endif
#include <fstream>

using namespace std;



class GUIwrapper{


public:
GUIwrapper(int nchans,int in,int event);
~GUIwrapper();

//slots
void OpenAux(int nchan);
void CloseWindow();
void NextEvt();
void PrevEvt();
void CloseAuxWindow();
void GetEvtByNumber();
void DrawIntegrals();
void DrawRawData();
void DrawGroupData(int group);
// void FitFromAuxWindow(int chan);


void display_data(bool changeevt);

private:

const int NCHANS;

int runin;
int event;
int fCurrentGroup;

bool display_integs;
bool display_rawdata;

MainFrame *fMainFrame;
AuxFrame *fAuxFrame;

process *analysis;
//the fit parameters

};


#endif
