#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "data.h"
#include "libDataIO.h"




int data_vCAEN_init(void);
int data_vCAEN_end(void);
int read_event_vCAEN(int*, int*, int*, char*, int*, int*, int*, int*);

extern int io_debug;

// int eh_size;

#define PMT_HEADER_SIZE  4  // in longwords

run_header1 rh1;
run_header2 rh2;
run_header3 rh3;
event_header2 eh2;
int eh2_size;
// event_header eh;

/* int io_number_of_pmt; */
/* int io_number_of_samples; */
/* int io_run_number; */

static int io_number_of_events;
static int io_hw_setup_id;
static int io_daq_version;
static int io_rh_size;

static int gzip_enabled;

int data_vCAEN_init(void) {
//io_debug=1;
  int rh_size,rh1_size,rh2_size,rh3_size;
  run_hh   rh;
  int ret;
  long t1,t2;

  rh_size = sizeof(rh);
  rh1_size = sizeof(rh1);
  rh2_size = sizeof(rh2);
  rh3_size = sizeof(rh3);
  eh2_size = sizeof(eh2);

  ret = fread(&rh, rh_size, 1, io_fp);
  if(ret != 1) {printf("Error reading run header !! \n"); return -1;}

  io_rh_size = rh_size;

  if(io_debug) {
    printf("rh.header_size =  %u \n",    rh.header_size);
    printf("rh.header_version =  %u \n", rh.header_version);
  }

   printf("hh header_size =  %u \n",    sizeof(rh));

   fseek(io_fp,-sizeof(rh),SEEK_CUR);

  if(rh.header_version == 100) {

    ret = fread(&rh1, rh1_size, 1, io_fp);
    if(ret != 1) {printf("Error reading run header !! \n"); return -1;}

    io_rh_size += rh1_size;

    if(io_debug) {
      t1 = rh1.date_time_start;
      t2 = rh1.date_time_stop;
      printf("rh.run_number =  %u \n",       rh1.run_number);
      printf("rh.date_time_start =  %s",     ctime(&t1));
      printf("rh.date_time_start =  %s",     ctime(&t2));
      printf("rh.daq_version =  %u \n",      rh1.daq_version);
      printf("rh.hw_setup_id =  %u \n",      rh1.hw_setup_id);
      printf("rh.number_of_pmt =  %u \n", rh1.number_of_pmt);
      printf("rh.number_of_samples =  %u \n", rh1.number_of_samples);
      printf("rh.number_of_events =  %u \n", rh1.number_of_events);
    }

    io_number_of_events = rh1.number_of_events;
    io_hw_setup_id      = rh1.hw_setup_id;
    io_number_of_pmt    = rh1.number_of_pmt;
    io_number_of_samples= rh1.number_of_samples;
    io_run_number       = rh1.run_number;
    io_daq_version      = rh1.daq_version;
  }

 
  if (rh.header_version == 110) {
    ret = fread(&rh3, rh3_size, 1, io_fp);
    if(ret != 1) {printf("Error reading run header !! \n"); return -1;}

    io_rh_size += rh3_size;

    if(io_debug) { 
      t1 = rh3.date_time_start;
      t2 = rh3.date_time_stop;
      printf("rh.run_number =  %u \n",       rh3.run_number);
      printf("rh.date_time_start =  %s",     ctime(&t1));
      printf("rh.date_time_start =  %s",     ctime(&t2));
      printf("rh.daq_version =  %u \n",      rh3.daq_version);
      printf("rh.hw_setup_id =  %u \n",      rh3.hw_setup_id);
      printf("rh.number_of_pmt =  %u \n", rh3.number_of_pmt);
      printf("rh.number_of_samples =  %u \n", rh3.number_of_samples);
      printf("rh.number_of_events =  %u \n", rh3.number_of_events);
      printf("rh.full_scale =  %u \n", rh3.full_scale);
      printf("rh.dc_offset =  %d \n", rh3.dc_offset);
      printf("rh.trig_delay =  %d \n", rh3.trig_delay);
    }

    io_number_of_events = rh3.number_of_events;
    io_hw_setup_id      = rh3.hw_setup_id;
    io_number_of_pmt    = rh3.number_of_pmt;
    io_number_of_samples= rh3.number_of_samples;
    io_run_number       = rh3.run_number;
    io_daq_version      = rh3.daq_version;
    io_adc_full_scale   = rh3.full_scale;
    io_pretrigger	= rh3.trig_delay*1000;   // to return number of samples
    io_is_caen 		 =true;
  }

  gzip_enabled = 0;
  if(io_hw_setup_id & 0x00000001) gzip_enabled = 1;

// printf("@@@@@@@@@@@@ reading gzip enabled: %d \n",gzip_enabled);

  
  
  if(gzip_enabled) warp_gunzip_init();
  else
  {
    // read to get event size
    ret = fread(&eh2, eh2_size, 1, io_fp); 
    fseek(io_fp, -eh2_size, SEEK_CUR);
  }
  
  
  return 0;
}

int data_vCAEN_end(void) {
  if(gzip_enabled) warp_gunzip_end();
  return 0;
}

const int    nnn = 4000000;
static short buffer[4000000];
static short    out[4000000];
static unsigned int event_index[100000];

int read_event_vCAEN(int *seve, int *run, int *eve,  char* times, int *event_time, int *nsamples, int *xtdf, int *ytdf) {

  const int magic_size = 4;
  int filepos, ret, nbout;
  int npmt, pointer, nsample, index;
  short *mout;
  int i,ipmt,isample;
  //acqiris_pmt_head *aqeh;
  long tt;

  
  if(*seve != 0) {

    if(gzip_enabled) {

      fseek(io_fp, -io_number_of_events*sizeof(event_index[0]), SEEK_END);
      fread(event_index, sizeof(event_index[0])*io_number_of_events, 1, io_fp);
      fseek(io_fp, event_index[(*seve -1)], SEEK_SET);

    } else {
//  printf(" %%%%%%%% sizes: magic %d io_rh: %d eh2_size: %d  event_size: %d \n",magic_size,io_rh_size,eh2_size,eh2.event_size);
      filepos = magic_size + io_rh_size + (*seve-1)*(eh2_size+eh2.event_size);
      fseek(io_fp, filepos, SEEK_SET);

    }

  }


  ret = fread(&eh2, eh2_size, 1, io_fp);
  
  
  if((ret != 1) || (eh2.header_version == 0)) {
    if(io_debug) 
      printf("Error reading event header for event ! \n");
    return -1;
  }

  if(io_debug) {
    tt = eh2.event_time;

    printf("eh2.header_size =  %u \n",    eh2.header_size);
    printf("eh2.header_version =  %u \n", eh2.header_version);
    printf("eh2.event_number =  %u \n",   eh2.event_number);
    printf("eh2.event_time =  %s",     ctime(&tt));
    printf("eh2.event_size =  %u \n",     eh2.event_size);
  }

  ret = fread(buffer, eh2.event_size, 1, io_fp);
  if(ret != 1) {printf("Error reading event %d ! \n", (int) eh2.event_number); return -1;}

  if(gzip_enabled) {
    nbout = warp_gunzip_data(eh2.event_size, (char *)&buffer[0], nnn, (char *)out);
    mout = out;
  } else {
    mout = buffer;
  }

  npmt = io_number_of_pmt;
  nsample = io_number_of_samples;

  *eve = eh2.event_number;
  *run = io_run_number;
  *nsamples = io_number_of_samples;
  *event_time = eh2.event_time;

  if(io_daq_version == 120) {

    pointer = 0; index = 0;
    for(ipmt=0; ipmt<npmt; ipmt++) {
     // aqeh = (acqiris_pmt_head*) &mout[pointer];
      //if(io_debug) {
//	printf("PMT board %u - channel %u \n", (aqeh->channel>>8)&0x000000FF,  aqeh->channel&0x000000FF);
//	printf(" data size = %d\n",  aqeh->data_size);
 //     }
   //   if(aqeh->data_size != io_number_of_samples) {printf("Event %d is corrupted !!", eh.event_number); return 2;}
      pointer+=PMT_HEADER_SIZE*8;   //x4 because it's four long words at 8bytes converted into shorts at 2.
     // printf( " ---- pointer %d   %d  \n ", pointer,PMT_HEADER_SIZE);
      //for(int xx=0;xx<6;xx++)
     // 	printf("point at %d  value:  %d \n",ipmt*nsample+100+xx*500,mout[ipmt*nsample+100+xx*500]);
      index = ipmt*nsample;
      for(isample=0; isample<nsample; isample++) ytdf[index + isample] = mout[pointer+isample];

      pointer += nsample;
    }  

  }

  if(io_debug) printf("WarpIO: %u samples \n",io_number_of_samples);

  if(times) ctime_r(&tt, times);

  for(i=0; i < nsample; i++) {
    xtdf[i] = i;
  }

  return 0;

}





