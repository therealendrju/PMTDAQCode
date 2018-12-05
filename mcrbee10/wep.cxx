
#include <iostream>
#include <fstream>
#include <sstream>

#include "wep.h"

#include "libDataIO.h"
#include "sql_interface.h"

#include <cstdio>

#define MAX_nSAMPLES 700000
#define MAX_nPMT        40

long int idx[38];

wep::wep() {

  debug = false; first_event = true;
  is_ser_found = false;
  isSer=false;
  isGUI=false;
  isAverage=false;

  DBread=false;
  DBrunread=0;

  nSAMPLES = 0;
  nPMT = 0;
  sampling = 1.0;
  fRefineBaseline = true;
  fUseDatabase = false;
  start_event = NULL; end_event = NULL;

  // for ser fitting
  chan_before=0;
  chan_after=0;
  integr_window=0;
  threshold=0;
  onset=0;
  duration=0;
  
  is_CAEN=false; 

  nRUN = 0; nEVENT = 0;

  data = (int*) malloc(4*MAX_nSAMPLES*MAX_nPMT);

  if(debug) io_set_debug();

}

wep::~wep(void) {

  free(data);

}

int wep::open_file(string s) {
  filename = s;
  warp_open((char*) filename.data());

  nRUN = io_run_number;
  nSAMPLES = io_number_of_samples;
  nPMT = io_number_of_pmt;
  vSCALE = io_adc_full_scale;
  preTRIG = io_pretrigger;	
  is_CAEN = io_is_caen;
  printf(" ----- pretrigger %d \n",preTRIG);

  for(int i=0; i<nPMT; i++) idx[i]=nSAMPLES*i;

  return 0;
}

int wep::close_file(void) {
  return warp_close();
}

int wep::readevent(int nnn) {

	/*
  if (isRawWaveforms == true) {
	// in raw waveform mode just using as a counter of number of events that exist
	return 0;
  }
*/

  int ret,nr,ne,ns;
  int dummy[800000];
  char time_string[26];
  int seq;

  seq = 0;
  if(nnn > 0) seq = (nnn-1)%1000 + 1;

  ret = read_event(&seq, &nr, &ne, &time_string[0], &event_time, &ns, &dummy[0], &data[0]);

  if( (ret < 0) || (ns != nSAMPLES) ) {
    if(debug) std::cout << "Error reading event !! ret = " << ret << " ns = " << ns << std::endl;
    return -1;
  }

  if(debug) std::cout << "Read Run# " << nr << "  Ev# " << ne << " Time = " << time_string << std::endl;


  //check that nRUN written in the file header corresponds to rn written in the event header






  if (nRUN != nr)
    {
      cout << "Possible problem! " << endl;
      cout << "Nrun = " << nRUN << " in the file header " << endl;
      cout << "and Nrun = " << nr << " in the header of event " << ne << endl;
      exit(1);
    }
  nEVENT = ne;

  return 0;
}

int wep::get_event(int run, int event) {

  char rrr[20];
  char eee[20];

  bool c1,c2,c3,n1;
  int e1,e2,offset;

  n1 = (nRUN == run) && (nEVENT == event);
  c1 = (nRUN == 0) && (nEVENT == 0);
  c2 = (nRUN != run);

  e1 = event - 1;
  e2 = nEVENT -1;

  c3 = (nRUN == run) && ((e1 - e1%1000) != (e2 - e2%1000));

  nRUN = run;
  nEVENT = event;

 // if( !n1 && (c1 || c2 || c3 )) {
 cout << " nrun" << run << endl;

    offset = 0;
    if( (nEVENT%1000==0) && (nEVENT>999) ) offset = 1000; 

    sprintf(rrr,"%.06d",nRUN);
    sprintf(eee,"%.06d",(nEVENT - nEVENT%1000 - offset));

    filename = "/data/Run";
    filename += rrr;
    filename += "/Run";
    filename += rrr;
    filename += "_";
    filename += eee;

    //    if(!c1) warp_close();
    //    warp_open((char*) filename.data());
    if(!c1) this->close_file();
    this->open_file(filename);
    
    if (isRawWaveforms == true) {
	// in raw waveform mode, don't get sers, just set all pmts to on to allow output of raw waveforms
	for (int i = 0; i < nPMT; i++) {
		isPMTOn.push_back(true);
	}
	is_ser_found = false;
    }
    else {
    	this->get_sers();
    }

	
  //}

  std::cout << "filename: " << filename << std::endl;

  for(int i=0;i<nPMT*nSAMPLES;i++)
  {
	  
	  data[i]=0;
  
  }
  
  //cout << "########zeroing data" <<endl;
  
  int ret,nr,ne,ns;
  int dummy[800000];
  char time_string[26];
  int seq;
  
  seq = e1%1000 + 1;

  ret = read_event(&seq, &nr, &ne, &time_string[0], &event_time, &ns, &dummy[0], &data[0]);

  if( (ret<0) || (nr != nRUN) || (ne != nEVENT) || (ns != nSAMPLES) ) {
//     if(debug) std::cout << "Error reading event !! ne = " << ne << std::endl;
    std::cout << "Error reading event !! ne = " << ne << " " << nEVENT << " nrun " <<nr<<" "<< nRUN<< " nsam " << ns <<" " << nSAMPLES << " +++ " << ret <<  std::endl;
    return -1;
  } else {
    std::cout << "Read Run# " << nr << "  Ev# " << ne << " Time = " << time_string << std::endl;
  }

  return 0;

}

int wep::process_event(void) {

  int nf,ret;

  first_event = false;

  if(start_event != NULL) this->start_event();

  ret = 0;
  nf = functions.size(); 
  for(int i=0; i<nf; i++) {
    ret += functions[i](this); 
    if(ret != 0) break;
  }

  if(end_event != NULL) this->end_event();

  return ret;
}

int wep::set_start(int (*f)(void)) {
  start_event = f;
  return 0;
}

int wep::set_end(int (*f)(void)) {
  end_event = f;
  return 0;
}

int wep::add_function(int (*f)(wep*)) {
  functions.push_back(f);
  return 0;
}

void wep::add_param(int a, string b, string s) {
  p = new parameter(a,b,s);
  parameter_vector.push_back(p);
}

void wep::get_param(int i, void* r) {

  int ii; float ff; char *ep=0; 

  switch(parameter_vector[i-1]->type) {
  case M_TYPE_FLOAT:
    ff = strtof(parameter_vector[i-1]->param.c_str(), &ep); 
    memcpy(r, &ff, sizeof(ff));
    break;
  case M_TYPE_INT:
    ii = strtol(parameter_vector[i-1]->param.c_str(), &ep, 10); 
    memcpy(r, &ii, sizeof(ii));
    break;
  default:
    break;
  }
}

int wep::get_database()
{

  if (!fUseDatabase) 
{
	for (int ipmt=0;ipmt<nPMT;ipmt++)
	{
	  isPMTOn.push_back(true);
	}

	return 0;
}
//std::cout << "DB test: use:" <<  fUseDatabase << " DBRunread: " << DBrunread << " nRun " << nRUN << std::endl; 


  if(!DBrunread || DBrunread!=nRUN)
	DBread=false;
  
//std::cout << "DBread: " << DBread << " | " << (!DBrunread) << " " << (DBrunread!=nRUN) << " || " << (!DBrunread || DBrunread!=nRUN) << std::endl;

 if(DBread)
	return 0;    // DB already read, no need to do it again.



  is_ser_found = false;
  
  //this method is supposed to read from database 
  //the active PMTs and the SERs
  //int nRUN=92001;
  //int nPMT=8;
  
   // following two lines added to clear the vectors. Andrzej
 


  PMTSer.resize(0);
  isPMTOn.resize(0);
  PMTSigmaSer.resize(0);
  PMTValley.resize(0);


  //Check that nRUN and nPMT are non-null
  if (!nRUN  || !nPMT)
    {
      cout << "Problem: the header does not contain the number of run and/or of PMT" << endl;
      cout << "No ser can be retrieved: assumed 7 PMT active and 1 ns*chan " << endl;
      nPMT = this->nPMT;
      for (int ipmt=0;ipmt<nPMT;ipmt++)
	{
	  isPMTOn.push_back(true);
	}
      is_ser_found = true;
      for (int ipmt=0;ipmt<nPMT;ipmt++)
	{
	  PMTSer.push_back(1.);
	  PMTSigmaSer.push_back(0.);
	}
      return 0;
    }
    
  cout << "Run " << nRUN << " has " << nPMT << " PMTs" << endl;

  //activate DB connection, create temporary arrays and retrieve from DB.
  char DBname[20];
#ifdef	__MY_SQL_TYPE__
	printf("__MY_SQL_TYPE__ defined \n");
	sprintf(DBname, "warp100");
#elif __MY_SQL_MC__
	printf("__MY_SQL_MC__ defined \n");
	sprintf(DBname, "warp_mc");
#else
	printf("__MY_SQL_TYPE__ not defined \n");
	sprintf(DBname, "warp_other");
#endif
  
  
  
  sql_interface DBinput(DBname);
  
  //std::exit(-1);
  
  results * rests=new results[nPMT];
  int *onoff=new int[nPMT];
  


  //The result is 0=not found,1=found
  int isresfound=DBinput.get_pmt_results(rests,onoff,nPMT,nRUN);  

  //for(int ipmt=0;ipmt<nPMT;ipmt++)
  //	onoff[ipmt]=1;

 
//cout << " is res found " << isresfound << " rests " << rests[0].xo<< " "<<vSCALE << endl;


//std::exit(-1);
 
  if (!isresfound)
    {
      cout << "The database does not contain data for run # " << nRUN << endl;
      cout << "No ser can be retrieved: assumed 1 ns*chan for all channels " << endl;
      for (int ipmt=0;ipmt<nPMT;ipmt++)
	{
	  isPMTOn.push_back(true);
	}
      is_ser_found = true;
      for (int ipmt=0;ipmt<nPMT;ipmt++)
	{
	  PMTSer.push_back(1.);
	  PMTSigmaSer.push_back(0.);
	  PMTValley.push_back(0.);
	  if (!isPMTOn[ipmt])
	    cout << "PMT #" << ipmt+1 << " is NOT active " << endl;
	}
      DBread=true;
      DBrunread=nRUN;  //if not found let's not connect a hundred times	
      return 0;
    }

  for (int ipmt=0;ipmt<nPMT;ipmt++)
    {
      isPMTOn.push_back(onoff[ipmt]);
    }
  
  for (int ipmt=0;ipmt<nPMT;ipmt++)
    {
      if (!isPMTOn[ipmt])
	cout << "PMT #" << ipmt+1 << " is NOT active " << endl;
    }

  //2) SERs
  is_ser_found = true;
  for (int ipmt=0;ipmt<nPMT;ipmt++)
    {
      PMTSer.push_back(rests[ipmt].xo);
      PMTSigmaSer.push_back(rests[ipmt].sigma);
      
    }
  
  //check that SERs of active PMTs are all valid (non-null!)
  for (int ipmt=0;ipmt<nPMT;ipmt++)
    {
      if (isPMTOn[ipmt] && (PMTSer[ipmt] < 1))
	is_ser_found = false;
    }
  
  
  //check if the valley has been inserted in the DB
  for (int ipmt=0;ipmt<nPMT;ipmt++)
    {
      if (rests[ipmt].valley == 0)
	{
	  cout << "No valley in the SER spectrum calculated for pmt " << ipmt << endl;
	  cout << "Take zero" << endl;
	}
      else
	{
	  cout << "Valley for PMT " << ipmt << " = " << rests[ipmt].valley << " chan" << endl;
	}
      PMTValley.push_back(rests[ipmt].valley);	
    }
  //if (!is_ser_found) return 0;
	  
  //final printout (if sers are found)
  for (int ipmt=0;ipmt<nPMT;ipmt++)
    if (isPMTOn[ipmt])
      cout << "SER for PMT #" << ipmt+1 << " = " << PMTSer[ipmt] 
	   << " ns*chan" << " on flag " << isPMTOn[ipmt] << " sigma " << rests[ipmt].sigma << " " 
	   << rests[ipmt].xo <<  endl;
  
  
  
  float parameters[4];
  
  int parexist=DBinput.get_ser_parameter_results(nRUN,parameters);
//cout << "---parameters " << parameters[0] << " " << parameters[1] << " " <<
//parameters[2] << " " << parameters[3] << endl; 

  DBread=true;
  DBrunread=nRUN;

  threshold=parameters[0];
  chan_before=parameters[1];
  chan_after=parameters[2];
  integr_window=parameters[3];
  



//std::exit(-1);
	   
  delete[] onoff;	
  delete[] rests;
  return 0;
}

// get sers from txt file
int wep::get_sers() {
	PMTSer.resize(0);
  	isPMTOn.resize(0);
  	PMTSigmaSer.resize(0);
  	PMTValley.resize(0);

	// open ser txt file, saved after running sercode
	stringstream ss;
	ss << "../sercode/ser_run" << nRUN << ".txt";
	string filename = ss.str();
	ifstream serFile(filename);
	if (serFile.is_open()) {
		is_ser_found = true;
		while (!serFile.eof()) {
			// read in line from file			
			int channel_num; double x0; double sigmax0; double valley; int onoff;
			serFile >> channel_num >> x0 >> sigmax0 >> valley >> onoff;
			// add ser values to wep object
			if (onoff == 0) { 			// pmt off
				isPMTOn.push_back(false);			
				PMTSer.push_back(0);
				PMTSigmaSer.push_back(0);
				PMTValley.push_back(0);			
			}
			else {					// pmt on
				isPMTOn.push_back(true);			
				PMTSer.push_back(x0);
				PMTSigmaSer.push_back(sigmax0);
				PMTValley.push_back(valley);
			}
		}
		serFile.close();
	}
	else {
		cout << "Error: cannot open ser file, sercode must be run first." << endl;
		is_ser_found = false;
		exit(1);
	}
 	return 0;
}



void wep::clear_functions(){
	functions.resize(0);
	return;
}






