#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <dirent.h>

#include "wep.h"
#include "processor_code.h"

using namespace std;


// function that counts the number of events looping through all files input 
int count_events (int argc, char **argv) {
	int num_events;

	// initalise wep object
	wep ppp;	
	ppp.set_start(start_ev);
	ppp.add_function(get_baseline);
	ppp.isRawWaveforms=true;	
	
	// count events
	// if input #events = -1, count the total number of events by looping through files given
	
	int ifile = 3;
	int iargc = argc-3;
	
	// count number of events
	int event_counter = 0;
	int ret = 0;
	do {
		ppp.open_file(argv[ifile++]);
		do {
			ret = ppp.readevent();	// read event just acts as counter in this case
			if (ret >= 0) event_counter++;
		} while (ret >= 0);
		ppp.close_file();
	} while(--iargc > 0);	
	// set number of events
	num_events = event_counter;
	cout << "Number of events found: " << num_events << endl;
	

	return num_events;
}

int main(int argc, char **argv) {    // input args: [run #events /data/Run######/*]
	
	int run = stoi(argv[1]);
	int num_events = stoi(argv[2]);

	// if #events = -1, count the total number of events
	if (num_events == -1) {
		num_events = count_events(argc, argv);
	}


	// initalise wep object
	wep ppp;
	
	ppp.set_start(start_ev);
	ppp.add_function(get_baseline);

	ppp.isRawWaveforms=true;	
	
	//get first event to extract number of samples
	int found = ppp.get_event(run, 1);
	
	const int nsamples = ppp.nSAMPLES;
	const int npmts = ppp.nPMT;

	// create output files for raw waveforms
	vector<ofstream> outFiles;	
	for (int ipmt = 0; ipmt < npmts; ipmt++) { 		
		stringstream ss;
		ss << "waveforms_ch" << ipmt << "_run" << run << ".txt";
		ofstream outFile(ss.str());
		if (outFile.is_open()) outFiles.push_back(move(outFile));
		else cout << "Could not open outfile: " << ss.str() << endl;		
		//string filename = ss.str();	
		//outFiles.emplace_back(ofstream{filename.c_str()});
		
	}

	
	// loop through events
	for(size_t i=0;i<num_events;i++) {
		// get event
		int found = ppp.get_event(run, i+1);
		
		if (found == 0) { 
			ppp.process_event(); 		
		
			for ( auto it = outFiles.begin(); it != outFiles.end(); ++it) {
			// add run number to each of the output files			
			*it << i+1 << " ";
			} 		
		
			for (int i=0;i<nsamples;i++) {
				for(int ipmt=0;ipmt<npmts;ipmt++) {
					// output of waveforms to text file
					outFiles[ipmt] << adccraw[i+ipmt*nsamples] << " ";  
				}
			}
			
			for ( auto it = outFiles.begin(); it != outFiles.end(); ++it) {
			// add endl to each of the output files			
			*it << endl;
			}
		}
	
	} //end event loop
	
	// close output files
	for ( auto it = outFiles.begin(); it != outFiles.end(); ++it) {
		// add endl to each of the output files			
		it->close();
	}
		

	return 0;
}
