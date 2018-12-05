Quick start readme for running PMTDAQCode:
For much more detail + descriptions of important functions refer to proper readme by Lan Nguyen, but note some parts have been updated since Lan's readme was written.
Patrick Green 12/2018

1. CAENdaq

./daq run_number events_per_run events_per_file number_of_runs number_of_pmt number_of_samples full_scale dc_offset trig_delay trig_level trig_source

- output files are saved in directory /data/Run#######/ 

2. wvformview

./wvformview number_waveforms_to_display run_number event_number

- shows plot of waveform from chosen run and event
- opens gui in which can select different events or runs / cycle through events in a run 

2. sercode

./newser nchan runnr -f

- calculates single phel response (ser)
- opens gui that allows you to fit exponential + n gaussians for each optical channel; first gaussian corresponds to single phel
- click save to file to save ser to .txt file named by run # that is then used for the later analysis (need to run sercode and save sers before running mcrbee), output file form:
	ch#	x0	sigma	valley	isPMTOn
- Note: if using ser from quick triggering run for analysis of pmt+tpc data, will need to manually change the output filename so run number matches pmt+tpc run number

3. mcrbee

./process output_file.root /data/Run######/*

- sercode must be run first
- reads in ser from sercode and calculates various useful parameters:
		spf = integral until 90ns
		sp = integral of entire pulse
		... (what else ?)
- outputs stored in root file specified. warning: if you don't specify output it overwrites first data file, be careful! 


./average_sig output_file.txt

- sercode must be run first
- calculates and plots average waveform for each channel and for all channels
- to use:
	a. use ttree and scan to make a file with columns of run # and events that are of interest:
		((TTreePlayer*)(tree->GetPlayer()))->SetScanRedirect(true);
		((TTreePlayer*)(tree->GetPlayer()))->SetScanFileName("output.txt");
		tree->Scan("run:event", "whatever conditions you like");
 			e.g. tree->Scan("run:event", "(spf[0]/sp[0] > 0.2 && spf[0]/sp[0] < 0.5)");
 	b. this will create text file that looks like: 
 		 *  0  *  1003  *  1  *
		 *  1  *  1003  *  2  *    
		 etc....
	   delete the first 3 lines and the last line, i.e. the lines that don't look like above, these break the ./average_sig input	
	c. run average_sig as above, histograms of average waveforms stored in root file called output_file.txt.root


./raw_waveform_output run_number number_of_events /data/Run######/*

- reads binary output data from ./CAENdaq in /data/Run######/ and outputs .txt file for each optical channel containing waveforms for each event. Each line contains event_number followed by entries of raw output voltage for each time sampled
- if input number_of_events = -1; counts the total number of events present and outputs all to the .txt files
- warning: creates large text files!
