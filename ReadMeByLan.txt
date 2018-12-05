How to use PMT DAQ CODE for Dummies:

by Lan Nguyen (18/7/2018)

There are 4 important files in analysis order: 
1. CAENdaq: control the CAEN Digitizer V1751 and run daq 
>> output: run # file in /data/

2. wvformviewer: to view waveforms for single event from run #

3. sercode: do integral over all the events from each run # and fill in histograms: SER(ADC*ns), amplitude(ADC), length(ns) 
>> output: run#.root, results_run#.dat, wvformresults_run#.dat

4. mcrbee10: process and  do average waveforms over all the events for each run #, with three waveforms: adccsum, adccraw, adccsumcorr
>> output: Run#.root, Run#.txt, Run#.txt.root


!!!!IMPORTANT!!!!
Before running these codes, make sure all the following libraries are up-to-date!
Currently: (18/7/2018)
CAENVMELIB 2.50
CAENComm 1.20
CAENDigitizer 2.11

Also, make sure to update the firmware of the digitizer!
Currently on V1751: (18/7/2018)
ROC FPGA 04.16 - Build 1720
AMC FPGA 00.07 - Build 0C02

####CAENdaq#######
################################################################################
To run the code, type:
./daq run_number events_per_run events_per_file number_of_runs number_of_pmt number_of_samples full_scale dc_offset trig_delay trig_level trig_source

run_number: the output file will be saved as Run######, with run_number = 0 - 999999
events_per_run: the number of events will be recorded for that run
events_per_file: the number of events will be saved in each file
number_of_runs: the number of runs for that daq
number_of_pmt: the number of pmt being used, currently hard-coded as 1 pmt only
number_of_samples: the number of samples in one event
full_scale (mV): for digitizer V1751, the full scale is 200mV
dc_offset (adc): set the baseline of the signal (0 - 1024)
trig_delay (us): the amount of time before the trigger, put -ve sign e.g. -2
trig_level (adc):  set the trigger threshold 
trig_source: the trigger source, currently hard-coded as 0 (can also understand as channel 0, for V1751, there are only 8 channels)

As the database is not currently available, the output file is saved at location: /data/

daq_main.c:

This first part of the code defines the arguments input from ./daq. At line 102 is where the arg[11] = trig_source is defined. For trig_source = 9, it can only be triggered if 3 or more channels are triggered at the same time.For trig_source = 10, the trigger is external. The usage of trig_source will be further explained in daq_pci.c

daq_pci.c:

This is the main code that sends command to control the digitizer V1751 so we can can change different parameters i.e. baseline, trigger delay, trigger threshold, etc. I will go through the code in line order, the line number might be different but it should be around that line.
(To see how to send command: see V1751 & VX1751 User Manual. To access the register address: see UM6009 - 751 Waveform Recording Firmware Registers Description. Find the manual from : http://www.caen.it/csite/CaenProd.jsp?idmod=602&parent=11#)

To turn on software trigger: uncomment line 610: //CAEN_DGTZ_SendSWtrigger(board_handle); 
The digitizer will automatically record any noise, signal regardless of trig level.

ln 157: defines how trig_delay is calculated such that: 
TrigDelay (%) = (number_of_samples+trig_delay*1000)/number_of_samples (%)
This TrigDelay value is the input of SetPostTriggerSize function, meaning how much of the total samples is after the trigger.
For examples, for number_of_samples = 20000, trig_delay=-2 (remember to input -ve), the TrigDelay = 90 %. Hence, 10% of the samples will be before the trigger and 90% of the samples will be after the trigger.

ln 169: set RecordLength = number_of_samples

ln 183: set triggering options
This is the logic for the trig_source input in daq_pci.c, the triggering option can be adjusted via register 0x810C
If trig_source < 9, it does nothing,  default as single channel trigger.
If trig_source = 9, only trigger if 3 channels are triggered.
If trig_source = 10, use external trigger.

ln 290: board calibration

ln 324: configuration for single channel, this enable mask for specific channel, or activate the channel number from trig_source input. There are two variable newmask and newmask2: newmask is used to activate the channel and newmask2 is used to set up triggers for that channel. For example, currently (3/8/2018), newmask is used to activate channel 0 and 1 while newmask2 is used to set trigger for channel 0 only.
 
This is written by me, hardcoded to measure signal triggered on channel 0 while measuring noise from empty channel 1.

ln 354: correction for baseline
the V1751 digitizer register only reads baseline in the range 0 - 65535 (2^16 - 1). However, Andrzej has it customized for SER signal in the range of 200mV only and baseline in the range 0 - 1023 (2^10 - 1). So this correction is such that when we input the baseline in 1023 range, it will be read in the 65535 range by the digitizer. It is a simple linear gradient with a gradient value and constant value.

ln 378: another correction for baseline, currently set at 0 so it does not do anything.

ln 394: set the trig_level = value to feed in the loop to configure channels

ln 406: a loop through all channels, only apply settings on channels that have their masks enabled in ln 324.
	- SetChannelDCOffset
	- SetChannelSelfTrigger, currently is hardcoded the same for both trig_source <9 and =9.
	- SetChannelTriggerThreshold
	- SetTriggerPolarity, currently set for falling edge (rising edge setting is commented)

The rest of the the daq_pci.c code is writing the data into files and onto the screen.

####wvformview#######
################################################################################

To run the code, type:
./wvformview ndisplay nrun nevent

ndisplay: the number of histograms to be displayed
nrun: runnumber
nevent: the event number

e.g. ./wvformview 1 1013 25 : show 1 histrogram of event 25 from Run001013


####sercode#######
################################################################################

To run the code, type:

Method 1: ./newser nchan runnr -f       

-f forces "recalculation". Basically, after the first run of the code the results of the analysis will be saved in a local .root file and the code will automatically load that in, unless you do -f. When modifying the code, do -f. This method can only read up to 5000 events.

Method 2: ./newser nchan runnr -i /data/Run00runnr/*   

This will use all the files in a directory. Remember to match directory with runnr. ;-) You can use other options, the -letter value  (where letters are defined in maingui). One possibly useful one might be:   -m XX    where XX is the threshold you want to use to select your single phel,defined in RMS of the baseline, so -m 3.0 should only take signals with amplitude above 3.0 RMS etc.

The sercode runs using the main engine of waveform analysis, which is currently in mcrbee10/processor_code.cxx. The processor_code.cxx is has different flag to do different things and parts dedicated to SER only will be inside of if statements like: if(p->isSER).

Let's go through some important features of the processor_code.cxx:

ln 52: define the three main histograms of the ser analysis: ser_hist[ADC*ns], ser_amp[ADC], and ser_leng[ns]. The range of these histograms are 0 - 300, no intrinsic reasons just that the phel peak was observed to be in this range.

To extract the ADC per phel, fit an exponential for noise and a set of gaussian, and assume the first gaussian corresponds to 1 phel. For energy, you need something with a well-defined energy deposition e.g. a radioactive source. Hence, you can get light yield (LY) in phel/energy.


ln 84: define the six branches in the ser_tree
npmt: number of pmt
tstart: time start
tstop: time stop
amp (same as ser_amp): amplitude of windows
length (same as ser_leng): length of windows
integ (same as ser_hist): ser integral

ln 175: function "term" to write data into histograms and ser_tree

ln 1388: MAIN PROCESS of the processor_code.cxx . Given this code was intially used in dark matter search then modified to study Argon scintillation, the pulses are defined as primary and secondary. For Argon, we are only interested in the primary pulses analysis and the secondary pulses analysis should not affect the primary at all. (If you run and check the spit out data, all secondary pulses size are zero).

	ln 1417: define sampling frequency = 1ns/samples (sometimes sample is refered as chan in the code)
	ln 1430: define pretrigger = the number of samples before trigger. Notice that 100 is subtracted from	the pretrigger, this is prevent any early part of the signal to be in the pretrigger. e.g. number of samples = 20,000 and trig delay = -2 us --> post trigger size = 90% --> pretrigger = 2000 - 100 = 1900
	ln 1573: Refine baseline
	The rest of the main process is to search for the location of the primary pulse and repeat the search to refine its location. If primary is found, then the loop will go the mega-function	CalculateIntegralsforPrimaries, aka the "Meat" of the SER calculation. This function in general takes a subregion of the whole waveform and looks for single phel peaks and calculates their integrals. And then does different things to them depending on the mode. 

ln 2491: Calculate Integrals For Primaries

In words, this function looks for any signal above threshold at minimum 1 ADC to form a window of 1ns, and add 5ns before and 25ns after the window. Then it looks to the left and right of this window, if spacing between window is smaller than 50ns or the window itself is small, consecutives windows will be merged together. These are the entries of the histograms for ser integral, amplitude and length. For ser, the entries are the areas of these windows. For amplitude, the entries are the peaks of the windows (the sample with the highest ADC value). For lenth, the entries are lenghts of the windows.

	ln 2513: defines window length
	ln 2514: defines number of samples before window = 5ns (1ns/sample)
	ln 2515: defines number of samples after window = 25ns 
	ln 2516: define threshold multiplier. Currently, the threshold multiplier is set as 3.
	ln 2563: define time window and ionset_primary. The standard, "proper" way to calculate SER in a real LAr Waveform would have been to set ionset_primary to ~3-4 us after the trigger to give the slow component time to mostly peter out and then catch single photoelectrons in the tails (still coming from slow component scintillation). 
	ln 2588: main loop on pmt, looping over all pmt. IsAboveThreshold is a boolean array and the final sample is always defined to be false. Any sample above threshold is defined to be true in this array.
	ln 2613: count the number of samples in the window. If the window is too shorter than minNumberOfSamplesAboveThreshold =1 , delete it. If the window is ok, add samples left and right.
	ln 2650: check spacing between windows. If it's smaller than window length = 50ns, merge two consecutives together.
	ln 2689: start integral calculation.This integral loops through all the windows defined as stated previously.
	ln 2716: for each window, the the code goes back 50ns then calculate the baseline to the left of the window.
	ln 2752: main integral of this mega function. There are three integral types: integral, fast integral and windown integral. The one we're interested in is just the normal integral. It is simply adding up all the ADC of the samples in the window together.
	ln 2753: ichanint counts how many times the samples of a window are added up, which corresponds to the length of the window.
	ln 2762: maxinwindow stores the highest ADC value (amplitude_ of a window.
	ln 2771: caculate the baseline 50ns to the right of the window.
	ln 2778: define the integral correction = 0.5*baseright*ichantin (Don't understand the correction, perhaps ask Andrzej for better explanation.)
	ln 2803: Record the data into the histograms and ser_tree. 

This is the gist of the ser_code. Once you run the code, you will get a window that is set up to do 40+ PMTs, but will show only one filled up.The fit will have spectacularly failed if there is no actual SER signal. Remember to click save file instead of save to DB (Database does not exist) to save file to current folder /sercode/. 

####mcrbee10#######
################################################################################
To run the code, type:
./process output.root /data/Run00100X/* 

Note that if you don't specify output.root it will overwrite the first data file from the list. That spits out a output.root file with different parameter that might be somewhat interesting. At first order probably noise, ppnoise and baseline will be only things that are actually reasonably defined, given the signal shape.

Using the ttree and the Scan command make a file with  colums of runnr and event:

((TTreePlayer*)(tree->GetPlayer()))->SetScanRedirect(true);
((TTreePlayer*)(tree->GetPlayer()))->SetScanFileName("output.txt");
tree->Scan("run:event");

This should create a file that looks like this:
*  0  *  1003  *  1  *
*  1  *  1003  *  2  *    
etc....

Remove the first three rows of the file and the last (basically those that don't look like that)and then run to get the average wvform:
./average_sig  output.txt

Same as sercode, the ./average_sig also runs using mcrbee10/processor_code.cxx. The flag in processor_code.cxx for average_sig  is (p->isAverage). Let's start with the tree definition in average_sig.cxx:

ln 153: defines three histogram: histo, integ and raw_histo. Then loop works such that it fill in the averaged waveform from all the channel into the three histogram, then repreat the same loop for individual channel for histo and raw_histo. Since we only measure 1 single channel, the histograms should be the same. 

By checking what arrays being filled into the histogram, we can track them backward to the processor_code.cxx:

rawhisto >> rawpulse[] >> adccraw[] : this is the raw data 
histo >> sumpulse[] >> adccsum[i] : this is the integral data without the correction
integ >> sumpulse_int[] >> adccsumcorr[] : this is the integral data with the correction (see ser code)


	







