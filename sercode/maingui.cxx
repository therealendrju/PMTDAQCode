#include "PMTwidget.h"

#include "GUIwrapper.h"
#include <TApplication.h>
#include <string>
#include <getopt.h>
//#include <map.h>
#include <string.h>
#include <sys/stat.h>


void create_standard_files(int run,vector<string> *files);





int main(int argc, char ** argv)
{



//



//TGMainFrame *MainFrame=  new TGMainFrame(gClient->GetRoot(),400,400);

double thresh=0;
int intbef=0,intaft=0,window=0;
vector<string> files;
int nchannels=0,nrun=0;

// Invokes ctor `GetOpt (int argc, char **argv, 
  //                       char *optstring);'
  //GetOpt getopt (argc, argv, "mbawfh:");

  if(argc<=2)
    return -1;
  
  nchannels=atoi(argv[1]);
  nrun=atoi(argv[2]);

  int option_char;
  
  std::map<string,double> parameters;
  
  bool forcecalc=false;  //force calculation (instead of using existing root file)
  
  int channel=0;
  char par=0;
 double value=0;
  
  // Invokes member function `int operator ()(void);'
  while ((option_char = getopt (argc,argv,":m:b:a:w:i:fx:s:d:h")) != EOF)
    switch (option_char)
      {  
         case 'm': parameters["threshhold"]= atof (optarg); break;
	 case 'b': parameters["intbef"]=atoi (optarg); break;
	 case 'a': parameters["intaft"]=atoi (optarg); break;
	 case 'w': parameters["window"]=atoi (optarg); break;
	 case 's': parameters["start"]=atoi (optarg); break;
	 case 'd': parameters["duration"]=atoi (optarg); break;
	 case 'f': forcecalc=true; std::cout << " found f" << std::endl; break;
// 	 case 'x':  channel=atoi(&optarg[0]);
// 		    par=optarg[1];
// 		    value=atof(&optarg[2]);
// 		   std::cout << "channel, param, value " << channel << " " << par << " " << value << std::endl;
// 		   break;
	 case 'i': optind--;
		   for( ;optind < argc && *argv[optind] != '-'; optind++){
		   files.push_back(argv[optind]);         
		  }
		  forcecalc=true;
		  break; 
	 case 'h':
         case '?': cout << argc << " " << !strncmp(argv[1],"-h",2) << endl;
		   cout << "### usage: ./newserXX ndisp nrun " << endl;
		   cout << "### where ndisp is the number of histograms to display" << endl;
                   cout << "### nrun is the run number (files assumed to be found in /pool)" << endl; 
                   cout << "#### optionally add: -o thresh intbefor intaft window" << endl;
		    cout << "#### thresh: threshold multiplier, intbefore: number of channels before signal" << endl;
		    cout << "#### intaft: number of channels after signal, window: minimum window between signals" << endl;
		    cout << "#### optionally add: -f list of files to be used" << endl;
		    cout << "#### As of now -o and -f cannot be used together." << endl;
		    std::exit(-1);
      }

   std:: cout << "testing maps: " << parameters["threshold"] << " bogus: "  <<parameters["andrzej"]  << " forcecalc " << forcecalc << std::endl;   
      
   std::cout << "second test " << parameters["threshold1"] << std::endl; 
      
GUIwrapper * hh;


if(forcecalc && files.size()==0 )    // forcing calculation but generic files not yet created.
  create_standard_files(nrun,&files);
else if(!forcecalc)                  //check whether .root file exists 
  {
  char fname[20];
  sprintf(fname,"run%d.root",nrun);
  struct stat stFileInfo;
   // Attempt to get the file attributes
  int intStat = stat(fname,&stFileInfo);
  if(intStat == 0) { 
    cout << "root file found, using that" << endl;
    files.push_back(fname);
    }
        
  }

for(int ix=0;ix<files.size();ix++)
  std::cout << "files used: " << files[ix] << std::endl;

			
//cout << " thresh " << thresh <<endl;
//PMTwidget *tt=new PMTwidget(gClient->GetRoot(),200,40);
TApplication theApp("Apqup",&argc,argv);
cout <<"running parameters " << argv[1] << " " << argv[2] << " " << thresh << endl; 
hh=new GUIwrapper(nchannels,nrun,files,parameters);


theApp.Run();


return 1;
}







void create_standard_files(int run,vector<string> *files)
{

  int initial=0;
  int iargc=0;
  int limit=5000;
  char path[20];
  
  int ninfile=1000;
 sprintf(path,"/data/Run%06d/",run);
 
   char argv[100];
 
 do{
 sprintf(argv,"%sRun%06d_%06d",path,run,iargc);
  files->push_back(argv);
 
  cout << argv << endl;

 iargc+=ninfile;	 
   // cout << iargc << " " << iargc-initial << endl;
  } while((iargc-initial) < limit);

}

