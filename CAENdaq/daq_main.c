#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _DAQ_MAIN_ 1

#include "daq_main.h"
#include "daq_runpar.h"
#include "daq_writer.h"

extern void* producer(void*);
extern void* consumer(void*);

#define NUM_THREADS 2

pthread_t tid[NUM_THREADS];      /* array of thread IDs */

// signal handling

void termination_handler (int signum) {
  if((signum == SIGUSR1) || (signum == SIGTERM)) {
    run_control.terminate=true;

    /* unlock threads..... */
    sleep(1);
    sem_post(&emptyBuffers);
    sem_post(&fullBuffers);
  }
}

int main(int argc, char *argv[]) {

  unsigned long *buffer;
  unsigned long buffer_size;
  int rc;
 
  char strpid[20];
  pid_t mypid;
  FILE *fl;

  printf ("Starting daq\n");

#ifdef _WARP_USE_SHM_

#ifdef WARP_USE_SHM_BUFFER 
  key_t shmbuf_sem_key, shmbuf_shm_key;
  unsigned short b_array[2];
#endif

#ifdef WARP_USE_SHM_DATA
  key_t shmdat_sem_key, shmdat_shm_key; 
  unsigned short d_array[2];
#endif

  // struct shmid_ds shmid_struct;
#endif

  struct sigaction new_action;
     
  /* Set up the structure to specify the new action. */

  new_action.sa_handler = termination_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
     
  sigaction (SIGUSR1, &new_action, NULL);
  sigaction (SIGTERM, &new_action, NULL);

  rc = 0;
  debug = false;

printf(" par 11_1:  \n ");

  if(argc < 10) {
    printf("Usage: \n");
    printf(" daq run_number events_per_run events_per_file number_of_runs number_of_pmt number_of_samples full_scale(mV) dc_offset(mV) trig_delay(us) \n");
  }

  run_control.run_number      = atoi(argv[1]);
  run_control.events_per_run  = atoi(argv[2]);
  run_control.events_per_file = atoi(argv[3]);
  run_control.number_of_runs  = atoi(argv[4]);

  daq_control.number_of_pmt     = atoi(argv[5]);
  daq_control.number_of_samples = atoi(argv[6]);

  daq_control.full_scale = atoi(argv[7]);
  daq_control.dc_offset  = atoi(argv[8]);
  daq_control.trig_delay = atoi(argv[9]);

 if(argc<=10)
  daq_control.trig_level=10;
  else
  daq_control.trig_level=atoi(argv[10]);
  

 if(argc<=11)
	{
	daq_control.trig_source=9;   //majority of 3
	//daq_control.trig_source=atoi(argv[11]);
   	}
  else
	{
	char * teststr=strstr(argv[11],"ch");
       	if(teststr!=NULL)
	  {
          daq_control.trig_source=argv[11][2]-'0';
	  }	
        else
	  {
          teststr=strstr(argv[11],"ext");
          if(teststr!=NULL)
	    daq_control.trig_source=10;
          else{
              daq_control.trig_source=9;  //default majority?
      	      }
	  }

	}

   daq_control.trig_source=0;
 printf("trigger source: %d \n",daq_control.trig_source);

//return 0;
//   if(daq_control.trig_source!=1 && daq_control.trig_source!=2 && daq_control.trig_source!=3 && daq_control.trig_source!=4)
//         {
//         printf("error, wrong trigger source, exiting\n");
//            exit(-1);
//         }




  printf("----------------------------------------\n");
  printf("Starting a new run\n");
  printf("run_number %ld \n",     run_control.run_number);
  printf("events_per_run %ld \n", run_control.events_per_run);
  printf("events_per_file %ld \n",run_control.events_per_file);
  printf("number_of_runs %ld \n", run_control.number_of_runs);
  printf("number_of_pmt %ld \n",    daq_control.number_of_pmt);
  printf("number_of_samples %ld \n",daq_control.number_of_samples);
  printf("full_scale %d mV \n",     daq_control.full_scale);
  printf("dc_offset %d mV \n",      daq_control.dc_offset);
  printf("trig_delay %d us \n",     daq_control.trig_delay);

  daq_control.number_of_buffers = 20;
  daq_control.event_size_32 = (daq_control.number_of_samples/4 + PMT_HEADER_SIZE*2)*daq_control.number_of_pmt;    // total event size (without header) in 32bit words  (4 longwords header for each pmt) + extra bytes in function Readout (daq_pci.c)

// change to "/2" to use short words instead of char. A.S.
 printf("main: event_size = %lu (longwords) \n",daq_control.event_size_32);

  run_control.enable_gzip = true;

  buffer_size = daq_control.event_size_32 * daq_control.number_of_buffers * 4 *2;  // in bytes  //*2 added to make 
  
  // changed t *2  from *4 because of change to shor arrays

  fl = fopen("daq.pid","r");
  if(fl != NULL) {
    printf("daq is already running or it crashed since /home/warp/daq.pid is still there !!\n");
    exit(-1);
  }

  fl = fopen("daq.pid","w");
  mypid = getpid();
  sprintf(strpid,"%8d \n",mypid);
  fwrite(strpid,10,1,fl);
  fclose(fl);

  if(debug) { 
    printf("main: run number = %lu \n" ,run_control.run_number);
    printf("main: number of events = %lu \n" ,run_control.events_per_run);
    printf("main: number of pmt = %lu \n" , daq_control.number_of_pmt);
    printf("main: number of runs = %lu \n" , run_control.number_of_runs);
    printf("main: event_size = %lu (longwords) \n",daq_control.event_size_32);
    printf("main: buffer_size = %lu (bytes) \n",buffer_size);
  }
  
  buffer = (long unsigned int*) malloc(buffer_size);
printf("++++++main: buffer_size = %lu (bytes) \n",buffer_size);
#ifdef _WARP_USE_SHM_
#ifdef WARP_USE_SHM_BUFFER

  /* keys for shared memory and semaphores */
  shmbuf_sem_key = (key_t) WARP_SHMBUF_SEM_KEY;
  shmbuf_shm_key = (key_t) WARP_SHMBUF_SHM_KEY;
  
  /* create 2 semaphores for shared memory buffer */
  shmbuf_semid = semget( shmbuf_sem_key, 2, 0666 | IPC_CREAT | IPC_EXCL );

  if( (shmbuf_semid == -1) && (errno == EEXIST) )
    shmbuf_semid = semget( shmbuf_sem_key, 2, 0666);

  if(shmbuf_semid == -1) {
    printf("main: shmbuf semget() failed\n");
    return -1;
  }

  /* initialize sem 0 (empty) to SHM_BUFFERS and sem 1 (full) to 0 */

  b_array[0] = 10;
  b_array[1] = 0;

  /* The '1' on this command is a no-op, because the SETALL command
     is used.                                                      */
  rc = semctl( shmbuf_semid, 1, SETALL, b_array);
  if(rc == -1) {
    if(debug) printf("main: shmbuf semctl() initialization failed\n");
    return -1;
  }
  //if(debug)
    printf("---------main: shared memory segment size (bytes) = %lu\n", buffer_size);
  shmbuf_shmid = shmget(shmbuf_shm_key, buffer_size, 0644 | IPC_CREAT | IPC_EXCL);

  if( (shmbuf_shmid == -1) && (errno == EEXIST) )
    shmbuf_shmid = shmget(shmbuf_shm_key, buffer_size, 0644);

  if (shmbuf_shmid == -1) {
    printf("main: shmbuf shmget() failed\n");
    return -1;
  }

  /* Attach the shared memory segment to the server process.       */
  shmbuf_address = shmat(shmbuf_shmid, NULL, 0);
  if ( shmbuf_address==NULL ) {
    printf("main: shmat() failed\n");
    return -1;
  }
#endif

#ifdef WARP_USE_SHM_DATA

  shmdat_sem_key = (key_t) WARP_SHMDAT_SEM_KEY;
  shmdat_shm_key = (key_t) WARP_SHMDAT_SHM_KEY;

  /* create 1 semaphore for shared memory data */

  shmdat_semid = semget( shmdat_sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL );

  if( (shmdat_semid == -1) && (errno == EEXIST) )
    shmdat_semid = semget( shmdat_sem_key, 1, 0666);

  if ( shmdat_semid == -1 && errno != EEXIST) {
    printf("main: shmdat semget() failed\n");
    return -1;
  }

  d_array[0] = 0;

  rc = semctl( shmdat_semid, 0, SETALL, d_array);
  if(rc == -1) {
    printf("main: shmdat semctl() initialization failed\n");
    return -1;
  }

  shmdat_shmid = shmget(shmdat_shm_key, SHM_DATA_SIZE, 0644 | IPC_CREAT | IPC_EXCL);

  if( (shmdat_shmid == -1) && (errno == EEXIST) )
    shmdat_shmid = shmget(shmdat_shm_key, SHM_DATA_SIZE, 0644);

  if (shmdat_shmid == -1) {
    printf("main: shmdat shmget() failed\n");
    return -1;
  }

  shmdat_address = shmat(shmdat_shmid, NULL, 0);
  if ( shmdat_address==NULL ) {
    printf("main: shmat() failed\n");
    return -1;
  }

#endif   /* WARP_USE_SHM_DATA */

#endif /* _WARP_USE_SHM_ */
  
  /* read/write semaphores*/

  sem_init(&emptyBuffers,   0, daq_control.number_of_buffers);
  sem_init(&fullBuffers,    0, 0);

  pthread_create(&tid[0], NULL, producer, &buffer[0]);
  pthread_create(&tid[1], NULL, consumer, &buffer[0]);

  pthread_join(tid[0], NULL);
 
  pthread_join(tid[1], NULL);
  
#ifdef WARP_USE_SHM_BUFFER
  rc = shmdt(shmbuf_address); //detach shared buffer 
  if (rc==-1) {
    printf("main: shmdt() failed\n");
    return -1;
  }
  if(debug) printf("main: shmbuf shmdt()\n");
#endif  

#ifdef WARP_USE_SHM_DATA 
  rc = shmdt(shmdat_address); //detach shared data
  if (rc==-1) {
    if(debug) printf("main: shmdat shmdt() failed\n");
    return -1;
  }
  if(debug) printf("main: shmdat shmdt()\n");
#endif

  free(buffer);

  system("rm daq.pid");

  printf("\n main:  reporting that all threads have terminated\n");
  /*   printf(" acquired  = %d \n",written_ev); */
  /*   printf(" processed = %d \n",read_ev); */
  /*   printf("      lost = %d \n",lost_ev); */
  /*   printf("  deadtime = %f \n",(1.0 - ((float) read_ev)/((float) (written_ev+lost_ev)))*100.0); */
  /*   printf("event rate = %f \n",1.0/(avtime/((float)(written_ev+lost_ev)))*1000000.0); */

  return 0;
}


