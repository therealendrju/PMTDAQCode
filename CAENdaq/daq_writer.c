#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "daq_main.h"

#include "daq_writer.h"
#include "daq_runpar.h"

#include "libDataIO.h"

#define max_event_size 2*1048576   //why?

const unsigned long magic_code = 0xFCD5;

void * consumer(void* bb)
{

  unsigned long *data = (unsigned long*) bb;
  unsigned long read_ev;
  unsigned long buffer_size_32;
  unsigned long raddr;
  //  acqiris_pmt_head *aqeh;

  int ev_index[100000];
  int nw;
//  int max_event_size = 1048576;
//  char wbuffer[max_event_size];
   char wbuffer[max_event_size]={0};
//  short int wbuffer[max_event_size]={0};
  char run_number_s[50];
  int rc;

  int ret;
  char command[50];

  FILE *fout;
  time_t s_time;

#ifdef WARP_USE_SHM_BUFFER
  unsigned long i;
  unsigned long shmwaddr;
  unsigned long* shm_w;

  shmwaddr=0;
#endif

  raddr=0;
  buffer_size_32 = daq_control.event_size_32 * daq_control.number_of_buffers;

  run_header rh;
  event_header  eh;

  if(run_control.enable_gzip) {
    rc = warp_gzip_init();
    if(rc != 0) {
      printf("consumer: FATAL !! Error in gzip_init() !!\n");
      return (void*) 0;
    }
  }

  time(&s_time);

 new_run:

/*   sprintf(command,"mkdir Run%.06lu", run_control.run_number); */
  sprintf(command,"mkdir /data/Run%.06lu/", run_control.run_number);
/*  sprintf(command,"mkdir /media/usbdisk/Run%.06lu/", run_control.run_number); */
  ret = system(command);
  if(ret != 0) {
    printf("consumer: FATAL !! Could not create output directory ( %s )\n", command);
    return (void*)0;
  }

  rh.header_size       = RUN_HEADER_SIZE*4;   // in bytes
  rh.header_version    = 110;
  rh.run_number        = run_control.run_number;
  rh.date_time_start   = s_time;
  rh.date_time_stop    = s_time;
  rh.daq_version       = 120;
  rh.hw_setup_id       = 200 + run_control.enable_gzip;
  rh.number_of_pmt     = daq_control.number_of_pmt;
  rh.full_scale        = daq_control.full_scale;
  rh.number_of_samples = daq_control.number_of_samples;
  rh.number_of_events  = 0;
  rh.dc_offset         = daq_control.dc_offset;
  rh.trig_delay        = daq_control.trig_delay;

  eh.header_size       = EV_HEADER_SIZE*4;   // in bytes    
  eh.header_version    = 100;
  eh.event_number      =   0;

  printf("====== WRITING SIZES: RUN_HEADER_SIZE: %d   ,sizeof: %d \n",rh.header_size,sizeof(rh));
  printf("====== WRITING SIZES: EVENT_HEADER_SIZE: %d   ,sizeof: %d \n",eh.header_size,sizeof(eh));

 new_file:

/*   sprintf(&run_number_s[9],"%.06lu_%.06lu",daqpar.runnum,eh.event_number); */
/*   sprintf(&run_number_s[0],"Run%.06lu_%.06lu", rh.run_number, eh.event_number); */

  sprintf(&run_number_s[0],"/data/Run%.06lu/Run%.06lu_%.06d", rh.run_number, rh.run_number, (int) eh.event_number);
/*  sprintf(&run_number_s[0],"/media/usbdisk/Run%.06lu/Run%.06lu_%.06d", rh.run_number, rh.run_number, (int) eh.event_number); */

  fout = fopen(run_number_s,"w");
  if(fout == NULL) {
    printf("consumer: FATAL !!! Could not open output file %s\n",run_number_s);
    return (void*)0;
  }

  // write dummy/incomplete header

  fwrite(&magic_code,4,1,fout);
  
  fwrite(&rh,rh.header_size,1,fout);

  read_ev = 0;

  do {

    sem_wait(&fullBuffers);
    if(run_control.terminate == true) break;

    if(debug) {
      printf("consumer: called with raddr = %lu \n",raddr);

      printf("consumer: -event written\t %lu\n", daq_control.events_written);
/*       printf("consumer: -emptyBuffers \t %d\n", emptyBuffers); */
/*       printf("consumer: -fullBuffers  \t %d\n", fullBuffers); */
/*       aqeh = (acqiris_pmt_head*) &data[raddr]; */
/*       printf("consumer: PCI board ID %lu - channel ID = %lu\n", (aqeh->channel>>8)&0x000000FF, aqeh->channel&0x000000FF); */
    }

    eh.event_number++;

    if(run_control.enable_gzip) {
      ev_index[read_ev]=ftell(fout);
       nw = warp_gzip_data(daq_control.event_size_32*8, (char*) (&data[raddr]), max_event_size, &wbuffer[0]); 
       //need to put in 8, because size of long is 8x char 
	//printf("consumer: daq_control_evt_size: %d, max_evt_size: %d \n",daq_control.event_size_32*8,max_event_size); 
//      nw = warp_gzip_data_s(daq_control.event_size_32*4, (short*) (&data[raddr]), max_event_size, &wbuffer[0]);
     // if(debug) 
   //   printf("consumer: nw = %d\n", nw);
      sem_post(&emptyBuffers);

/*       eh.event_size = eh.header_size + nw;  // in bytes */
      eh.event_size = nw;  // in bytes

      if(debug) { 
	  printf("consumer: eh.header_size =  %u \n",    eh.header_size);
	  printf("consumer: eh.header_version =  %u \n", eh.header_version);
	  printf("consumer: eh.event_number =  %lu \n",   eh.event_number);
	  printf("consumer: eh.event_time =  %lu \n",     eh.event_time);
	  printf("consumer: eh.event_size =  %lu \n",     eh.event_size);
      }

      time((time_t*) &eh.event_time);
      fwrite(&eh, eh.header_size, 1, fout);
      fwrite(&wbuffer[0], eh.event_size, 1, fout);
    } else {
      //      eh.event_size = (EV_HEADER_SIZE + daq_control.event_size_32)*4;
      //      eh.event_size = daq_control.event_size_32*4; // 300408
      // change to *2 from *4 to compensate the chenge to short
     
      if(debug) { 
      printf("consumer: eh.header_size =  %u \n",    eh.header_size);
	  printf("consumer: eh.header_version =  %u \n", eh.header_version);
	  printf("consumer: eh.event_number =  %lu \n",   eh.event_number);
	  printf("consumer: eh.event_time =  %lu \n",     eh.event_time);
	  printf("consumer: eh.event_size =  %lu \n",     eh.event_size);
      }
      
      eh.event_size = (daq_control.event_size_32)*4;
      time((time_t*) &eh.event_time);
      fwrite(&eh, eh.header_size, 1, fout);
      fwrite(&data[raddr], eh.event_size, 1, fout);
      sem_post(&emptyBuffers);
    }
    
    if( (eh.event_number%1000)==0 ) printf("consumer: wrote event %ld \n", eh.event_number); 

#ifdef WARP_USE_SHM_BUFFER

    operation[0].sem_num = 0; /* Operate on the empty sem     */
    operation[0].sem_op = -1; /* Decrement the semval by one   */
    operation[0].sem_flg = IPC_NOWAIT;
    
    rc = semop( shmbuf_semid, operation, 1 );
    if (rc >= 0) {
	
      if(debug) printf("consumer: rc OK for event %lu \n",daq_control.events_written);
	
      shm_w = ((unsigned long*) shmbuf_address);
	
      if(debug) printf("consumer: shmwaddr = %lu\n", shmwaddr);
      if(debug) printf("consumer: shm_w = %p\n", shm_w);

      for(i=0; i<daq_control.event_size_32; i++)
	{
	  shm_w[shmwaddr + i]=data[raddr + i];
	}
	
      shmwaddr += daq_control.event_size_32;
      if(shmwaddr == buffer_size_32) shmwaddr=0;
	
      operation[0].sem_num = 1; /* Operate on the full sem     */
      operation[0].sem_op =  1; /* Increment the semval by one   */
      operation[0].sem_flg = 0;
	
      rc = semop( shmbuf_semid, operation, 1 );
	
    } else {
      if(debug) printf("consumer: rc < 0 for event %lu\n", daq_control.events_written);
    }
    
#endif

    raddr += daq_control.event_size_32;
    if(raddr==buffer_size_32) raddr=0;

/*     eh.event_number++; */
    read_ev++;
    daq_control.events_written++;

    if(debug) printf("consumer: read addr= %lu       read ev = %lu \n",raddr, read_ev);
    if(debug) printf("consumer: written ev= %lu      terminate = %u \n", daq_control.events_written, run_control.terminate);

  } while (read_ev != run_control.events_per_file);

  if(run_control.enable_gzip) fwrite(ev_index, sizeof(ev_index[0])*read_ev, 1, fout);

  rewind(fout);

  fwrite(&magic_code, 4, 1, fout);

  rh.number_of_events = read_ev;

  time(&s_time);
  rh.date_time_stop = s_time;

  fwrite(&rh, rh.header_size, 1, fout);

  fclose(fout);

  if(run_control.terminate == true) goto term;

  if(eh.event_number == run_control.events_per_run) {
    eh.event_number=0;
    if(--run_control.number_of_runs) {
      run_control.run_number++;
      goto new_run;
    }
  } else {
    goto new_file;
  }

 term:

  if(run_control.enable_gzip) {
    rc = warp_gzip_end();
    if(rc != 0) printf("consumer: FATAL !! Error in gzip_end() !!\n");
  }

  printf("consumer: exiting (terminate = %u)\n", run_control.terminate);

  run_control.terminate = true;

  /* unlock producer if buffer is full....*/

  sleep(1);
  sem_post(&emptyBuffers);

  return (void *)0;
}   
