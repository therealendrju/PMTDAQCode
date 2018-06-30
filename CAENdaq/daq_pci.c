/*
 * daq_pci.c
 *  
 *
 *  Created by A.Cocco
 *  Adapted by Sandro Ventura on 1/28/08 to handle Acquiris boards
 *  Adapted by A. Szelc to handle the CAEN V1751 board
 */

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#include "daq_main.h"
#include "daq_runpar.h"
#include "daq_writer.h"
#include "daq_pci.h"
#include <math.h>
#include <stdio.h>


#include <CAENDigitizer.h>

//int kNumBoards;

// ### Global variables ###
//ViSession InstrumentID[10];
// Array of instrument handles
//long NumInstruments;
// Number of instruments
int board_handle;





const long number_of_segments = 1;

// #define kNumBoards 4   // defined in daq_pci.h

//////////////////////////////////////////////////////////////////////////////////////////




static long get_time()
{
	long time_ms;
#ifdef WIN32
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
	struct timeval t1;    
	struct timezone tz;
	gettimeofday(&t1, &tz);
	time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
	return time_ms;
}





void FindDevices(void) {

   // int npmts=daq_control.number_of_pmt;

//  kNumBoards=(npmts+npmts%2)/2;

  // Find all digitizers
 // options = "cal=1 dma=1";
CAEN_DGTZ_ErrorCode status=CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_PCI_OpticalLink,0,0,0,&board_handle);
if (status) {
		printf("error in open nr %d",status);
		exit(status);
	}



CAEN_DGTZ_BoardInfo_t info;

CAEN_DGTZ_GetInfo(board_handle,&info);

if (status) {
		printf("error in info nr %d",status);
		exit(status);
	}


    printf("Connected to CAEN Digitizer Model %s\n", info.ModelName);
    printf("ROC FPGA Release is %s\n", info.ROC_FirmwareRel);
    printf("AMC FPGA Release is %s\n", info.AMC_FirmwareRel);
    

status =CAEN_DGTZ_Reset(board_handle);
if (status) {
		printf("error in Reset nr %d",status);
		exit(status);
	}





}


inline void StopAcq(char * bufferCAEN,CAEN_DGTZ_UINT16_EVENT_t    *Event16) {

CAEN_DGTZ_ErrorCode status;
CAEN_DGTZ_SWStopAcquisition(board_handle);

if( Event16) {
	      CAEN_DGTZ_FreeEvent(board_handle, (void**)&Event16);
		}

CAEN_DGTZ_FreeReadoutBuffer(&bufferCAEN);
status = CAEN_DGTZ_CloseDigitizer(board_handle);
printf("stopping Acquisition with status: %d\n",status);
}


//////////////////////////////////////////////////////////////////////////////////////////
void Configure() {

  // ### Configuration ###
CAEN_DGTZ_ErrorCode status=0;
  //double FullScale, Offset, 
  double TrigDelay;

 

 // double sampInterval = 1.e-9;



  //FullScale = 1.0e-3*((double) daq_control.full_scale);
  //Offset = 1.0e-3*((double) daq_control.dc_offset);

  
  
  uint32_t posttrig_reg;

CAEN_DGTZ_ReadRegister(board_handle,0x8114,&posttrig_reg);
printf("----------------------------------");
printf("Post Trig Number of Samples %d \n",posttrig_reg);


  TrigDelay = ((double)(daq_control.number_of_samples+daq_control.trig_delay*1000))/(double)(daq_control.number_of_samples)*100;
	//TrigDelay
  uint32_t readTrig;
  printf("Trig Delay %d %f \n",(int)TrigDelay,TrigDelay);

  status |= CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig);
  printf("&&&&&&&& ReadTrig before %d \n",readTrig);
  
  //  TrigDelay = 1.0e-9*((double) daq_control.trig_delay);
  //TrigDelay = 1.0e-6*((double) daq_control.trig_delay);

status |= CAEN_DGTZ_SetDESMode(board_handle, 0);
status |= CAEN_DGTZ_SetRecordLength(board_handle, daq_control.number_of_samples);
status |= CAEN_DGTZ_SetPostTriggerSize(board_handle, (int)TrigDelay);
status |= CAEN_DGTZ_SetIOLevel(board_handle, 0);   // 1= TTL, 0=NIM

  status |= CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig);
 printf("&&&&&&&& ReadTrig after %d \n",readTrig);


CAEN_DGTZ_ReadRegister(board_handle,0x8114,&posttrig_reg);
printf("----------------------------------");
printf("Post Trig Number of Samples %d \n",posttrig_reg);



  //////////////set triggering options:

if(daq_control.trig_source<9){} 
//single channel trigger
else if(daq_control.trig_source==0) { // majority trigger on 3  

//if(daq_control.trig_source==0) { // majority trigger on 3 
uint32_t  data_reg;  


CAEN_DGTZ_ReadRegister(board_handle,0x810C,&data_reg);
printf("Trigger register, after zero set %ld,%ld,%ld \n",(long int)data_reg & 1<<26,(long int)data_reg & 1<<25,(long int)data_reg & 1<<24 );
printf("coincidence width register, after zero set %ld,%ld,%ld \n",(long int)data_reg & 1<<22,(long int)data_reg & 1<<21,(long int)data_reg & 1<<20 );

//set majority to 3.
data_reg&=~(1<<24);
data_reg|=1<<25;
//data_reg&=~(1<<25);
data_reg&=~(1<<26);

//set coincidence width to 8ns
data_reg|=1<<20;
data_reg&=~(1<<21);
data_reg&=~(1<<22);
data_reg&=~(1<<23);

CAEN_DGTZ_WriteRegister(board_handle,0x810C,data_reg & ~(1<<1));

CAEN_DGTZ_ReadRegister(board_handle,0x810C,&data_reg);
printf("Trigger register, after zero set %ld,%ld,%ld \n",(long int)data_reg & 1<<26,(long int)data_reg & 1<<25,(long int)data_reg & 1<<24 );
printf("coincidence width register, after zero set %ld,%ld,%ld \n",(long int)data_reg & 1<<22,(long int)data_reg & 1<<21,(long int)data_reg & 1<<20 );



}
else if(daq_control.trig_source==10) // external trigger
{

status |= CAEN_DGTZ_SetExtTriggerInputMode(board_handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY); 




}

 /////////////////calibration
 ////  Set Broadcast bit to 0, then to 1.
  uint32_t  data_reg;  
/*  // read ADC Broadcast bit
  CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);

//printf("Broadcast register 1 %ld \n",(long int)data_reg);

printf("Broadcast register  %ld \n",(long int)data_reg & 1<<1 );

//setting to zero
CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);

//printf("Broadcast register 3 %ld \n",(long int)data_reg);


CAEN_DGTZ_WriteRegister(board_handle,0x809C,data_reg & ~(1<<1));

CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);

printf("Broadcast register, after zero set %ld \n",(long int)(data_reg & 1<<1) );
 
//setting to one.
CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);

//printf("Broadcast register 4 %ld \n",(long int)data_reg);

CAEN_DGTZ_WriteRegister(board_handle,0x809C,data_reg | 1<<1);

CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);

sleep(1);

printf("Broadcast register, after one set %ld \n",(long int)(data_reg & 1<<1) );
 
//polling channels to see if calibration ends	     

int flag=0;
int j;

while(flag < 512)
{ flag=0;
	for(j=0;j<8;j++)
	{
 	CAEN_DGTZ_ReadRegister(board_handle,0x108+j*256,&data_reg);
	printf("Channel %d, adress %x register %ld \n",j,0x1088+j*256,(long int)data_reg & 1<<6);
	flag+=data_reg & 1<<6;
	}
//
}

printf("flag %d \n",flag);
CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);
CAEN_DGTZ_WriteRegister(board_handle,0x809C,data_reg & ~(1<<1));

CAEN_DGTZ_ReadRegister(board_handle,0x809C,&data_reg);
printf("Broadcast register, after second zero set %ld \n",(long int)data_reg & 1<<1 );
 
*/
  
CAEN_DGTZ_Calibrate(board_handle);

////////////////////////////////  
int NumEvts=1;
status |= CAEN_DGTZ_SetMaxNumEventsBLT(board_handle, NumEvts);
status |= CAEN_DGTZ_SetAcquisitionMode(board_handle, CAEN_DGTZ_SW_CONTROLLED);
//status |= CAEN_DGTZ_SetExtTriggerInputMode(board_handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);  
//status |= CAEN_DGTZ_SetExtTriggerInputMode(board_handle, CAEN_DGTZ_TRGMODE_EXTOUT_ONLY);    


///////////////////////////////////// write register to set negative majority calculation

// set 6th bit to 1 to allow negative thresholds

CAEN_DGTZ_ReadRegister(
             board_handle,
             0x8000,
             &data_reg);
printf("register %ld \n",(long int)data_reg);
data_reg |= 1<<6;

CAEN_DGTZ_WriteRegister(
             board_handle,
             0x8000,
             data_reg);

CAEN_DGTZ_ReadRegister(
             board_handle,
             0x8000,
             &data_reg);
printf(" reread register %ld \n",(long int)data_reg);
	     


//////////////////// single channel configurations

int newmask=0;
int i=0;
bool DESmode=false;
int DESmultiplier=1;
if(DESmode)
{  i=1;
   DESmultiplier=2;
}
for(;i<daq_control.number_of_pmt*DESmultiplier;i+=DESmultiplier)	newmask|= (1 << i);
//for(i;i<daq_control.number_of_pmt*DESmultiplier;i+=1)	newmask|= (1 << i);

printf("++++++ new mask %d \n",newmask);
printf("------ daq_control.trig_level 0 %d \n",daq_control.trig_level);

//long int offs=0x8900;
//int DCoffset[8]={offs-230,offs-250,offs-125,offs-350,0,0,0,0};
//long int offs=daq_control.dc_offset*10;
//offs = (int)((offs+50) * 65535 / 100);


//long int offs=8500;
//long int offs=daq_control.dc_offset*10;
//long int offs2=(daq_control.dc_offset-910)*15/1.13;

//printf("------ offs/offs2 0 %d %d \n",offs,offs2);

//offs = (long int)((offs+50) * 65535 / 100)-offs2;

//Correction for DCoffset input to output

float corr_grad = -13.442200; //for pmt
float corr_const= 44453;	//for pmt	

//float corr_grad = -13.3397; //for pulser
//float corr_const= 44778.3;  //for pulser

printf("------ corr_grad/  %f  \n",corr_grad);
printf("------ corr_const/  %f  \n",corr_const);

long int offs=daq_control.dc_offset*corr_grad+corr_const;

//long int triglevel = daq_control.trig_level*corr_grad+corr_const;
//printf("-------- triglevel/ 0 %ld \n", triglevel);

//long int offs=daq_control.dc_offset*10;

printf("------ offs/  %ld  \n",offs);

float corr_fact=15/1.13;

//printf("------ corr_fact/ 0 %f  \n",corr_fact);

int corrections[8]={0.,0.,0.,0.,0.,0.,0.,0.};
//int corrections[4]={0};

//changed DCoffset from long int to float
long int DCoffset[8]={offs-corrections[0]*corr_fact,offs-corrections[1]*corr_fact,offs-corrections[2]*corr_fact,offs-corrections[3]*corr_fact,offs-corrections[4]*corr_fact,offs-corrections[5]*corr_fact,offs-corrections[6]*corr_fact,offs-corrections[7]*corr_fact};
//////////////////////////////////////////////////////////////////////////////	
/////////trigger settings	


//signal should be at 0 volts, so trigger is at FSR[ADC] - DCoffset[ADC] - trig [ADC]
//int value=1023-5*50-7;

//long int value=daq_control.dc_offset-daq_control.trig_level;
//long int value = 43444;//triglevel;
//value = 70;

//long int value=daq_control.trig_level*10;
long int value=daq_control.trig_level;
uint32_t valueout;


printf("------ daq_control.dc_offset  %d \n",daq_control.dc_offset);

printf("------ daq_control.trig_level  %d \n",daq_control.trig_level);

printf("------ value  %ld \n",value);

printf("------ DCOffset  %ld \n",DCoffset[i]);
 
status |= CAEN_DGTZ_SetChannelEnableMask(board_handle, newmask);
	for(i=0; i<8; i++) {
        	if (newmask & (1<<i)) {
		printf("Setting DC Offset, channel %d to %ld \n", i,DCoffset[i]);
                status |= CAEN_DGTZ_SetChannelDCOffset(board_handle, i, DCoffset[i]);
		//status |= CAEN_DGTZ_SetChannelSelfTrigger(board_handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, (1<<i));
		//status |= CAEN_DGTZ_SetChannelSelfTrigger(board_handle,CAEN_DGTZ_TRGMODE_EXTOUT_ONLY , (1<<i));

		if(daq_control.trig_source==9){
		status |= CAEN_DGTZ_SetChannelSelfTrigger(board_handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY , (1<<i));
		printf("I am here %d",i);
		}
		else if (daq_control.trig_source<9){
			if(i==daq_control.trig_source)	
//		    	CAEN_DGTZ_SetChannelSelfTrigger(board_handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT , (1<<i));
  printf("_----- are we here? \n");              
		status |= CAEN_DGTZ_SetChannelSelfTrigger(board_handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY , (1<<i));
		}

		printf("---------------------------------- \n");
		printf("Setting DC Offset, channel %d to %ld \n",i,DCoffset[i]);

		printf("------ daq_control.trig_source, %d \n", daq_control.trig_source);
		printf("------ daq_control.trig_level, channel %d, %d \n",i,daq_control.trig_level);
		printf("------ value, channel %d, %ld \n",i,value);

	status |= CAEN_DGTZ_GetChannelTriggerThreshold(board_handle, i, &valueout); 
		
		printf(" threshold value before setting, channel %d, %d \n",i,valueout);

                status |= CAEN_DGTZ_SetChannelTriggerThreshold(board_handle, i, value); 

                status |= CAEN_DGTZ_SetTriggerPolarity(board_handle, i, CAEN_DGTZ_TriggerOnFallingEdge);
		//status |= CAEN_DGTZ_SetTriggerPolarity(board_handle, i, CAEN_DGTZ_TriggerOnRisingEdge);
		
		//status |= CAEN_DGTZ_SetPostTriggerSize(board_handle, (int)TrigDelay);

		printf("---------------------------------- \n");
		printf("Setting DC Offset, channel %d to %ld \n",i,DCoffset[i]);
		printf("------ daq_control.trig_source, %d \n", daq_control.trig_source);
		printf("------ daq_control.trig_level, channel %d, %d \n",i,daq_control.trig_level);
		printf("------ value, channel %d, %ld \n",i,value);
		
		printf("setting channel %d threshold to: %ld \n",i,value);
		
		status |= CAEN_DGTZ_GetChannelTriggerThreshold(board_handle, i, &valueout); 
		
		printf("threshold value after setting, channel %d, %d \n",i,valueout);

				   }
			    }

		

  		status |= CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig);
 		printf("&&&&&&&& ReadTrig after %d \n",readTrig);

		//uint32_t posttrig_reg;
		//status |= CAEN_DGTZ_ReadRegister(board_handle,0x8114,&posttrig_reg);
		//printf("----------------------------------\n");
		//printf("Post Trig Number of Samples %ld \n",posttrig_reg);
		
 //new uncomment, A. 05.07.13
//CAEN_DGTZ_SetChannelSelfTrigger(board_handle,CAEN_DGTZ_TRGMODE_EXTOUT_ONLY , mask);

}

 // CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig);
 //printf("&&&&&&&& ReadTrig after %d \n",readTrig);
//////////////////////////////////////////////////////////////////////////////////////////
void StartAcquisition(char * bufferCAEN) {

uint32_t AllocatedSize;  
  
CAEN_DGTZ_SWStartAcquisition(board_handle);

CAEN_DGTZ_MallocReadoutBuffer(board_handle, &bufferCAEN,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */

}

int CheckTrigger(char * bufferCAEN,uint32_t *BufferSizeCAEN) {

  // ### Acquiring a waveform ###
  CAEN_DGTZ_ErrorCode ret=0;
  uint32_t NumEvents=0;
  
  		
  
  
  
  
  long timeoutCounter = 200000; // Timeout waiting for a trigger
  long errTimeoutCounter = 500000; // Timeout for acquisition completion

  while (!NumEvents && errTimeoutCounter--) {
  ret |= CAEN_DGTZ_ReadData(board_handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, bufferCAEN, BufferSizeCAEN);
  ret |= CAEN_DGTZ_GetNumEvents(board_handle, bufferCAEN, *BufferSizeCAEN, &NumEvents);
 
      if (!timeoutCounter--) // Trigger timeout occured
	printf("Slow trigger...\n");
      		//	AcqrsD1_forceTrig(InstrumentID[mboard]); // Force a 'manual' (or 'software') trigger
  }
  return NumEvents;
}

//////////////////////////////////////////////////////////////////////////////////////////


void *producer(void* bb) {

  unsigned long *data = (unsigned long*) bb;
  unsigned long buffer_size_32;
  //unsigned int timsta1,timsta2;
//  long evsiz;

  
  
  char *bufferCAEN = NULL;
  
  CAEN_DGTZ_ErrorCode ret=0;
/*   unsigned int h3,h4; */

  //mod by Andrzej 27.06.2007
 // time_t start=time(NULL);
  //end mod
  
  
#ifdef WARP_USE_SHM_DATA
  unsigned long* shm_w;
  int rc;
#endif

  unsigned long waddr;
  long totsiz;

  /*   float tt, rate; */
  /*   unsigned long mytime; */

  FindDevices(); // ### Initialize the Digitizer ###

   
  /* Clear Output Buffer */

  buffer_size_32 = daq_control.event_size_32 * daq_control.number_of_buffers;
 printf(" ------------- producer: event_size_32 = %lu, nbuffers: %d\n",daq_control.event_size_32,daq_control.number_of_buffers);
  waddr = 0;
  daq_control.events_readout = 0;

  if(debug)
    printf(" ------------- producer: event_size_32 = %lu\n",daq_control.event_size_32);

  /* start DAQ */

 
  Configure();
  
  printf("producer: START DAQ \n");
 
  //StartAcquisition(bufferCAEN);
 uint32_t AllocatedSize;  
//  int nevts=0;
  int Nb=0,Ne=0,Netot=0;
  uint64_t CurrentTime, PrevRateTime=0, ElapsedTime;
  uint64_t FirstTime;
  int nCycles= 0;
  uint32_t NumEvents;
  char *EventPtr = NULL;
  CAEN_DGTZ_EventInfo_t       EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t    *Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  uint32_t BufferSizeCAEN;
 
 
// start the daq 
ret |= CAEN_DGTZ_AllocateEvent(board_handle,(void **)&Event16);
ret |= CAEN_DGTZ_MallocReadoutBuffer(board_handle, &bufferCAEN,&AllocatedSize);
ret |= CAEN_DGTZ_SWStartAcquisition(board_handle);  
  
  printf("producer: started acquisition \n" );


  // extra array to be ale to write as short
  const int nsam=daq_control.number_of_samples*2;
unsigned short * sdata= (unsigned short*) malloc(nsam);
  
  
  FirstTime = get_time(); 
  CurrentTime = get_time(); // putting it here to avoid warning
  do {

 // printf("producer: in loop \n" );

 

    if(debug) {
      printf("producer: +event readout\t %lu\n", daq_control.events_readout);

    }


    if(debug) printf("producer: producer called  \n");

    totsiz=0;
    int i = 0;
///////////////////////start CAEN readout
		/* Read data from the board */
		//CAEN_DGTZ_SendSWtrigger(board_handle);

		//status |= CAEN_DGTZ_GetChannelTriggerThreshold(board_handle, 0, value); 		
		//printf("------value %ld \n",i,value2);

		   /* Get number of stored events */
		  if((NumEvents=CheckTrigger(bufferCAEN,&BufferSizeCAEN))==0) continue;
		
		
		

		/* Calculate throughput and trigger rate (every second) */
		Nb += BufferSizeCAEN;
		Ne += NumEvents;
		Netot += NumEvents;
		CurrentTime = get_time(); 
		ElapsedTime = CurrentTime - PrevRateTime;

		nCycles++;
		if (ElapsedTime > 1000) {
			if (Nb == 0)
				printf("No data...\n");
				}
		if (ElapsedTime > 5000 && Nb!=0) {	
				printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
			nCycles= 0;
			Nb = 0;
			Ne = 0;
			PrevRateTime = CurrentTime;
		}

             //  printf(" *** event number: %i \n",Ne);
	//	printf("number of evts  %d , %d", Ne,nevts);

		/* Analyze data -> send to consumer */ 
		for(i = 0; i < (int)NumEvents; i++) {

		     //printf("producer: got num events %d \n",NumEvents);
		      if(sem_trywait(&emptyBuffers) != 0) continue;

		      if(run_control.terminate == true) break;

		  
			/* Get one event from the readout buffer */
			ret = CAEN_DGTZ_GetEventInfo(board_handle, bufferCAEN, BufferSizeCAEN, i, &EventInfo, &EventPtr);
			if (ret) {
				exit(ret);
			}
			if((ret=CAEN_DGTZ_DecodeEvent(board_handle, EventPtr, (void**)&Event16))){
					exit(ret);
					}
					
			


			if(Event16==NULL) continue;
			/* Write Event data to file */

		      int ch=0,j=0;
// 			for(ch=0;ch<4;ch++)
// 			{
// 				fprintf( fout, "BID: % 4d  EVT: % 8d  PTN: %08x  TTT: % 8d\n", 
// 						EventInfo.BoardId,
// 					//	EventInfo.ChannelEventSize,
// 					//	EventInfo.ChannelId,
// 						EventInfo.EventCounter,
// 						EventInfo.Pattern,
// 						EventInfo.TriggerTimeTag
// 						);
// 			}   // end of loop on channels
                        
			  
			 bool DESmode=false;
			 int DESmultiplier=1;
			  if(DESmode)
			  {ch=1;
			  DESmultiplier=2;
			  } 
					
			 for(;ch<daq_control.number_of_pmt*DESmultiplier;ch+=DESmultiplier)
				{
				  //transcribe channel header
				   data[waddr  +totsiz]=0x01000000 | (1<<8) | ch;  //board id
				   data[waddr+1+totsiz]=daq_control.number_of_samples;
				    /* 	  data[waddr+2+totsiz]= h3; */
				    /* 	  data[waddr+3+totsiz]= h4; */
				    data[waddr+2+totsiz]=1;
				    data[waddr+3+totsiz]=9;
				    
				    // trick to write data as short instead of long
				    sdata=&data[waddr+PMT_HEADER_SIZE*2+totsiz];
//				printf("event16 cell size: %d , sdata element: %d , data size: %d\n",sizeof(Event16->DataChannel[ch][j]),sizeof(sdata[0]),sizeof(data[0]));
//				  printf(" evsiz = %ld  totsiz = %ld daq_control_size: %d\n", evsiz, totsiz,daq_control.event_size_32);
//				 printf(" ----  ev16_size,waddr  %d %lu %ld total_size: %ld \n",Event16->ChSize[ch],waddr,totsiz,waddr+PMT_HEADER_SIZE*2+totsiz+Event16->ChSize[ch]);
				 //transcribe data to each channel
  //                               printf("nsamples: %d  : ch: %d \n",daq_control.number_of_samples, ch);
  			
	        

		//printf("reading out for chan: %d : event size: %d \n",ch,(int)Event16->ChSize[ch]);

// 				 int xx;
// 				 for(xx=0;xx<6;xx++)
//       				    printf("point at channel: %d, at %d  value:  %d \n",ch,100+xx*500,Event16->DataChannel[ch][100+xx*500]);

				 for(j=0; j<(int)Event16->ChSize[ch]; j++) 
					  {
					   //data[waddr+PMT_HEADER_SIZE+totsiz+j]=Event16->DataChannel[ch][j];
					   sdata[j]=Event16->DataChannel[ch][j];
					  // printf("Reading out chan %d, sample %d, value %d \n",ch,j,sdata[j]);	
					//int readTrig2;
					//CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig2);
 					//printf("&&&&&&&& ReadTrig2 after %d \n",readTrig2);
					  }
				 // printf("2 reading out for chan: %d : event size: %d \n",ch,(int)Event16->ChSize[ch]);	
 				//for(xx=0;xx<6;xx++)
      				//    printf("sdata point at channel: %d, waddr %d at  %d  value:  %d \n",ch,waddr+PMT_HEADER_SIZE*2+totsiz,100+xx*500,sdata[100+xx*500]);                                    

				     totsiz+=daq_control.number_of_samples/4 + PMT_HEADER_SIZE*2;
				}	

			uint32_t readTrig2;
			CAEN_DGTZ_GetPostTriggerSize(board_handle, &readTrig2);
 			printf("&&&&&&&& ReadTrig2 after %d \n",readTrig2);

			//uint32_t posttrig_reg;

			//CAEN_DGTZ_ReadRegister(board_handle,0x8114,&posttrig_reg);
			printf("----------------------------------\n");
			//printf("Post Trig Number of Samples %d \n",posttrig_reg);

			  sem_post(&fullBuffers);

			  waddr += daq_control.event_size_32;
			  if(waddr >= buffer_size_32) waddr = 0;
			  daq_control.events_readout++;

			  //if(debug)
			  //  printf("producer:   write addr= %lu       write ev = %lu,  buffer = %lu \n",waddr, daq_control.events_readout,buffer_size_32);	
					  
    
		}  //end of loop on NumEvts
/////////////////////stop CAEN readout


#ifdef WARP_USE_SHM_DATA
      
    operation[0].sem_num = 0; /* Operate on the sem     */
    operation[0].sem_op =  0; /* Check for zero         */
    operation[0].sem_flg = IPC_NOWAIT;

    operation[1].sem_num = 0; /* Operate on the sem     */
    operation[1].sem_op =  1; /* Increment              */
    operation[1].sem_flg = IPC_NOWAIT;
			
    rc = semop( shmdat_semid, operation, 2 );
    if (rc >= 0) {

      /*     if(debug) printf("rc OK for event %d\n",read_ev); */

      shm_w = ((unsigned long*) shmdat_address);
      shm_w[0] = daq_control.events_readout;
      shm_w[1] = daq_control.events_written;
      //added by Andrzej 27.06.2007
      time_t now=time(NULL);
      shm_w[2] = run_control.run_number;
      shm_w[3] = (long)start;
      shm_w[4] = (long)now;
      //end of mod.
      /*       shm_w[1] = daqpar.nevents; */
      /*       shm_w[2] = lost_ev; */

      /*       if(debug) */
      /* 	{ */
      /* 	  printf("\n shm_w[0] = %d \t", shm_w[0] ); */
      /* 	  printf("\t shm_w[1] = %d \t", shm_w[1] ); */
      /* 	  printf("\n shm_w[2] = %d \n", shm_w[2] ); */
      /* 	} */

      shm_w[5] = daq_control.number_of_pmt;
      shm_w[6] = daq_control.number_of_samples;
      shm_w[7] = daq_control.number_of_buffers;
      shm_w[8] = daq_control.full_scale;
      shm_w[9] = daq_control.dc_offset;
      shm_w[10] = daq_control.trig_delay;

      operation[0].sem_num = 0; /* Operate on the sem     */
      operation[0].sem_op = -1; /* Decrement the semval by one   */
      operation[0].sem_flg = IPC_NOWAIT;

      rc = semop( shmdat_semid, operation, 1 );
    }

#endif

  } while (run_control.terminate == false);

  printf("producer: exiting (terminate = %u)\n", run_control.terminate);
ElapsedTime = CurrentTime - FirstTime;
printf("Reading at (Trg Rate: %.2f Hz)\n", (float)Netot*1000.0f/(float)ElapsedTime);
  /* Stop DAQ */

  FILE * rateout=fopen("rates.txt","a");
  fprintf(rateout,"%ld %.2f Hz \n",run_control.run_number,(float)Netot*1000.0f/(float)ElapsedTime);
/*   StopAcq(); */
 fclose(rateout);

  StopAcq(bufferCAEN,Event16);

 // free(sdata);
  printf("producer: STOP DAQ \n");

  return (void*)0;

}
