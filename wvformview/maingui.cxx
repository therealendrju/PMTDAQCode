#include "PMTwidget.h"
#include <stdlib.h>
#include "GUIwrapper.h"
#include <TApplication.h>








int main(int argc, char ** argv)
{

//	cout << "test " << endl; 

/*	int ac=3;	
	char ** arv;
	for(int i=0;i<4;i++)
		arv[i]=new char[12];
	arv[0]="newser100";
	arv[1]="16";
	arv[2]="1208";	*/	
	//arv[3]="1";	
	//char ** arvv=&arv;
TApplication theApp("Apqup",&argc,argv);



//TGMainFrame *MainFrame=  new TGMainFrame(gClient->GetRoot(),400,400);


if(argc<=3 || !strncmp(argv[0],"-h",2))
{
cout << "### usage: ./wvformview ndisp nrun nevent " << endl;
cout << "### where ndisp is the number of histograms to display" << endl;
cout << "### nrun is the run number (files assumed to be found in /pool)" << endl;
cout << "### nevent is the event number to be displayed" << endl;
std::exit(-1);
}


int run=0,event=0; 

if(argc>2) run=atoi(argv[2]);
if(argc>3) event=atoi(argv[3]);

//PMTwidget *tt=new PMTwidget(gClient->GetRoot(),200,40);
cout <<"running parameters " << argv[1] << " " << argv[2] << endl; 
GUIwrapper * hh=new GUIwrapper(atoi(argv[1]),run,event);

theApp.Run();

delete hh;
return 1;
}
