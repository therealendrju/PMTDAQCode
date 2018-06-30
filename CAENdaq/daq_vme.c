#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "daq_main.h"

#include "daq_vme.h"
#include "daq_runpar.h"
//mod by Andrzej 27.06.2007
#include <time.h>
//end mod.

extern bool debug;

unsigned long blt_size;

extern void *memcpy(void*, const void*, size_t);

int daq_v2718_init(void) {

  CVBoardTypes  VMEBoard;
  short         Link;
  short         Device;
  int fmode;

  VMEBoard = cvV2718;
  Device = 0;
  Link = 0;

  if( CAENVME_Init(VMEBoard, Device, Link, &BHandle) != cvSuccess ) {
    printf("\n\n Error opening the device\n");
    return -1;
  }

  fmode = 0;
  if( CAENVME_SetFIFOMode(BHandle, fmode) != cvSuccess ) {
    printf("\n\n Error setting FIFO mode\n");
    return -1;
  }

  return 0;

}

int daq_v1724_init(void) {

  int ret,i;
  unsigned long address;
  unsigned long out;
  unsigned long value;

  if(debug) printf("daq_v1724_init() \n");

  /* Software Reset Register */
  address = BaseAddr | 0xEF24;
  value = 0x0;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Software reset register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Software Clear Register */
  address = BaseAddr | 0xEF28;
  value = 0x0;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Software clear register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Trigger Source Enable Mask */
  address = BaseAddr | 0x810C; 
  value = 0x40000000;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Trigger Source Enable Mask \t\t %s ",(ret==cvSuccess)?"OK":"Failed");
  ret = CAENVME_ReadCycle(BHandle,address,&out,cvA32_S_DATA,cvD32);
  if(debug) printf(" \t %lx \n",out);

  /* Trigger Out Enable Mask */
  address = BaseAddr | 0x8110; 
  value = 0x0;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Trigger Out Enable Mask \t\t %s ",(ret==cvSuccess)?"OK":"Failed");
  ret = CAENVME_ReadCycle(BHandle,address,&out,cvA32_S_DATA,cvD32);
  if(debug) printf(" \t %lx \n",out);

  for(i = 0; i < 8; i++) {    
    /* DAC Data Configuration Register */
    address = ( BaseAddr | 0x1000 | ( i * 0x100) ) | 0x98; 
    
    if(i%2 == 0) {
      value = 0x240000;
    } else {
      value = 0x100000;
    }
    value |= 0x1800;

    /*     value = 0x00341800; */
    
    ret  = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);      
    ret += CAENVME_ReadCycle(BHandle,address,&out,cvA32_S_DATA,cvD32);
    if(debug) {
      if(ret != cvSuccess) printf("daq_v1724_init: DAC data configuration ret = %d \n",ret);
      printf("daq_v1724_init: DAC data configuration DATA[%d] = %lx \n", i, out & 0x00FFFFFF);
    }
  }

  /* Channel Configuration Register */
  address = BaseAddr | 0x8000; 
  value = 0x34;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Channel Configuration Register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Buffer Size Register */
  address = BaseAddr | 0x800C; 
  value = 0x5;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Buffer Size Register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Post Trigger */
  address = BaseAddr | 0x8114; 
  value = 0x5;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Post Trigger \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Front Panel I/O */
  address = BaseAddr | 0x811C; 
  value = 0x0;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Front Panel I/O \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* Channel Enable Mask */
  address = BaseAddr | 0x8120; 
  value = 0xFF;
  if(daq_control.number_of_pmt == 4) value = 0x0F;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: Channel Enable Mask \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /* VME Control Register */
  address = BaseAddr | 0xEF00; 
  value = 0x10;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  if(debug) printf("daq_v1724_init: VME Control Register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed");

  /*   if(debug) { */
  /*     /\* Firmware Revision *\/ */
  /*     address = BaseAddr | 0x8124; */
  /*     ret = CAENVME_ReadCycle(BHandle,address,&value,cvA32_S_DATA,cvD32); */
  /*     if(ret != cvSuccess ) printf("daq_v1724_init: Firmware revision %lx ret = %d \n", address, ret); */
  /*     printf("daq_v1724_init: Firmware revision %lx  val =  %lx \n", address, value); */

  /*     /\* VME Status *\/ */
  /*     address = BaseAddr | 0xEF04; */
  /*     ret = CAENVME_ReadCycle(BHandle,address,&value,cvA32_S_DATA,cvD32); */
  /*     if(ret != cvSuccess ) printf("daq_v1724_init: VME status %lx ret = %d \n", address, ret); */
  /*     printf("daq_v1724_init: VME status %lx  val =  %lx \n", address, value); */

  /*     /\* ???? *\/ */
  /*     address = BaseAddr | 0xEF34;  */
  /*     value = 0x0; */
  /*     ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32); */
  /*     if(debug) printf("daq_v1724_init: VME Control Register \t\t %s \n",(ret==cvSuccess)?"OK":"Failed"); */

  /*     /\* ROM Content *\/ */
  /*     for(i=0; i<0x88; i+=0x4) { */
  /*       address = BaseAddr | 0xF000 | i; */
  /*       ret = CAENVME_ReadCycle(BHandle,address,&value,cvA32_S_DATA,cvD32); */
  /*       if(ret != cvSuccess ) printf("daq_v1724_init: ROM address %lx ret = %d \n", address, ret); */
  /*       printf("daq_v1724_init: ROM address %lx  val =  %lx \n", address, value); */
  /*     } */
  /* } */

  return cvSuccess;

}

void * producer(void* bb) {

  unsigned long *data = (unsigned long*) bb;
  unsigned long value, address, buffer_size_32;
  unsigned long header[4];

  //mod by Andrzej 27.06.2007
  time_t start=time(NULL);
  //end mod
  
  
#ifdef WARP_USE_SHM_DATA
  unsigned long* shm_w;
  int rc;
#endif

  unsigned long waddr;
  int ret, count;

  /*   float tt, rate; */
  /*   unsigned long mytime; */

  ret = daq_v2718_init();
  if(ret != cvSuccess ) {
    printf("producer: Init 2718 failed !! ret = %d \n", ret);
  } else {
    printf("producer: Init 2718 OK \n");
  }

  ret = daq_v1724_init();
  if(ret != cvSuccess ) {
    printf("producer: Init 1724 failed !! ret = %d \n", ret);
  } else {
    printf("producer: Init 1724 OK \n");
  }

  /* Clear Output Buffer */

  address = BaseAddr | 0x8010; 
  value = 0x20;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  printf("producer: Clear Output Buffer  ret = %d \n",ret);

  buffer_size_32 = daq_control.event_size_32 * daq_control.number_of_buffers;
  blt_size = daq_control.event_size_32 * 4;

  waddr = 0;
  daq_control.events_readout = 0;

  if(debug) printf("producer: event_size_32 = %lu\n",daq_control.event_size_32);

  /* start DAQ */

  address = BaseAddr | 0x8100; 
  value = 0xC;        /* count all triggers (bit 3) */
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  printf("producer: START DAQ  ret = %d \n",ret);

  do {

    /* Get number of stored events */
    address = BaseAddr | 0x812C;
    ret = CAENVME_ReadCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
    if(ret != cvSuccess ) {
      printf("producer: Events stored %lx ret = %d \n", address, ret);
    }

    if(value == 0) continue;

    if(debug) {
      printf("producer: +event readout\t %lu\n", daq_control.events_readout);
      printf("producer: +Event ready  \t %lu\n", value);
/*       printf("producer: +emptyBuffers \t %d\n", emptyBuffers); */
/*       printf("producer: +fullBuffers  \t %d\n", fullBuffers); */
    }

    /* Write BLT number of stored events to readout */

    address = BaseAddr | 0xEF1C;
    ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
    if(ret != cvSuccess ) {
      printf("producer: BLT Events stored %lx ret = %d \n", address, ret);
    }

    do {

      /* try to read header */

      address = BaseAddr;
      ret = CAENVME_FIFOBLTReadCycle(BHandle,address,&header[0],
				     16, cvA32_U_BLT,cvD32, &count);

      if((ret == cvBusError) || (count != 16)) continue;

      sem_wait(&emptyBuffers);

      /*     if(!sem_trywait(&emptyBuffers)) { */

      if(run_control.terminate == true) break;

      if(debug) printf("producer: producer called with waddr = %lu \n",waddr);

      memcpy(&data[waddr], &header[0], 16);

      /*       address = BaseAddr; */

      // block transfer of data
      ret = CAENVME_FIFOBLTReadCycle(BHandle,address,&data[waddr+4],
				     blt_size-16, cvA32_U_BLT,cvD32, &count);
      if(count != blt_size-16) printf("producer: Error address mismatch !!!  %d    %d   %lu\n", ret, count, blt_size);
      if(debug) printf("producer: BLT ret = %d\n", ret);

      sem_post(&fullBuffers);

      waddr += daq_control.event_size_32;
      if(waddr == buffer_size_32) waddr = 0;

      daq_control.events_readout++;

      if(debug) printf("producer:   write addr= %lu       write ev = %lu \n",waddr, daq_control.events_readout);

      /*     } else { */
      /*       if(run_control.terminate == true) break; */
      /*     } */

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

	operation[0].sem_num = 0; /* Operate on the sem     */
	operation[0].sem_op = -1; /* Decrement the semval by one   */
	operation[0].sem_flg = IPC_NOWAIT;

	rc = semop( shmdat_semid, operation, 1 );
      }

#endif

    } while(ret != cvBusError);

  } while (run_control.terminate == false);

  printf("producer: exiting (terminate = %u)\n", run_control.terminate);

  /* Stop DAQ */

  address = BaseAddr | 0x8100; 
  value = 0x8;
  ret = CAENVME_WriteCycle(BHandle,address,&value,cvA32_S_DATA,cvD32);
  printf("producer: STOP DAQ  ret = %d \n",ret);

  return (void*)0;

}

