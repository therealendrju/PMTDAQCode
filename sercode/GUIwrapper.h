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
#include <map>
#include <string>
//#include <string>
//#include <vector>

using namespace std;



class GUIwrapper{


public:
// GUIwrapper(int nchans,int in,vector<string> files,float thresh=0.0,int bef=0,int aft=0,int win=0);
GUIwrapper(int nchans,int in,vector<string> files,std::map<string,double> params);
~GUIwrapper();

//slots
void OpenAux(int nchan);
void CloseWindow();
void SaveDB();
void SaveFile();
void BatchFit();
void CloseAuxWindow();
void FitFromAuxWindow(int chan);
void ChangeDisplay(int mode);
void ChangeNGaussInAuxWindow();

void initialize_data();

private:

	
void draw_data();	
	
const int NCHANS;
int runin;
int runout;
vector<string> filelist;

float threshold;
std::vector<float> threshold_per_pmt;
int before_window;
int after_window;
int window;

int auxchannel;

int display_mode;   // which mode to display ser, amplitude or length
bool already_fitted;

MainFrame *fMainFrame;
AuxFrame *fAuxFrame;

process *analysis;
//the fit parameters

};


#endif
