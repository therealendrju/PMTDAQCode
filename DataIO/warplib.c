#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _WARPLIB_
#include "libDataIO.h"

//int icarus_init(void) {return 0;};
//int icarus_end(void) {return 0;};
//int read_event_icarus(int* a, int* b, int* c, char* d, int* e, int* f, int* g, int* h) {return 0;};


extern int data_vCAEN_init(void);
extern int data_vCAEN_end(void);
extern int read_event_vCAEN(    int*, int*, int*, char*, int*, int*, int*, int*);
//extern int warp_close_vCAEN(void);

// extern int warp_open_v4(char*);
// extern int warp_close_v4(void);
// extern int read_event_v4(    int*, int*, int*, char*, int*, int*, int*, int*);
// 
// extern int warp_open_100_v1(char*);
// extern int warp_close_100_v1(void);
// extern int read_event_100_v1(    int*, int*, int*, char*, int*, int*, int*, int*);
// 
// extern int read_event_mc(    int*, int*, int*, char*, int*, int*, int*, int*);
// extern int write_event_mc(   int, int, int, int, int, int, unsigned int*);

void io_set_debug(void) { io_debug = 1; }

static int wmode;
static int data_format;

int warp_wopen(int iwmode, char* infile) {

  char cmd[256];
  unsigned int wid;
  int size;

 // wmode = iwmode;
 // if(wmode==icarus) return -1;

  strcpy(cmd, "gzip -c > ");  /* use "gzip -c" for zwrite */
  strncat(cmd, infile, sizeof(cmd)-strlen(cmd));
  
  io_fw = popen(cmd, "w");  /* use "w" for zwrite */
  if (io_fw == NULL) {
    fprintf(stderr, "WarpIO: popen('%s', 'w') failed\n", cmd);
    return -1;
  }

//   if(wmode == mc_v1) {
//     wid = 0xFCA1;
//     size = sizeof(wid);
//     fwrite(&wid, size, 1, io_fw);
//     write_event = write_event_mc;
//   }
// 
//   if(wmode == mc_v2) {
//     wid = 0xFCA2;
//     size = sizeof(wid);
//     fwrite(&wid, size, 1, io_fw);
//     write_event = write_event_mc;
//   }

//   if(wmode == warp_v1) {
//     wid = 0xFCD1;
//     size = sizeof(wid);
//     fwrite(&wid, size, 1, io_fw);
//     write_event = write_event_data;
//   }
  
  return 0;
}

int warp_wclose(void) { 
  int ret;
  ret =pclose(io_fw);
  return ret;
}

int warp_open(char* infile) {


  short n1,n2;
  char cmd[256];
  unsigned int id;
  int size, rc;

  io_fp=fopen(infile,"r");
  n1 = fgetc(io_fp);
  n2 = fgetc(io_fp);
  fclose(io_fp);

  io_is_compressed = 0;

  if( (n1 == 31) && (n2 == 139) ) io_is_compressed = 1;

  
  if(io_is_compressed) {

    if(io_debug) {
      printf("WarpIO: Data in input file is compressed\n");
      printf("Opening %s\n", infile);
    }

    strcpy(cmd, "gzip -dc ");  /* use "gzip -c" for zwrite */
    strncat(cmd, infile, sizeof(cmd)-strlen(cmd));

    printf(" Command = %s \n", cmd);
  
    io_fp = popen(cmd, "r");  /* use "w" for zwrite */
    if (io_fp == NULL) {
      fprintf(stderr, "WarpIO: popen('%s', 'r') failed\n", cmd);
      return -1;
    }

  } else {

    if(io_debug) {
      printf("WarpIO: Data in input file is not compressed\n");
      printf("Opening %s\n", infile);
    }
    io_fp=fopen(infile,"r");

  }

  id = 0;
/*   if(!io_is_compressed) { */
    size = sizeof(id);
    rc = fread(&id,size,1,io_fp);
/*   } */

  if(io_debug) printf("WarpIO: -- Data id = %.8x \n", id);

  data_format = warp_v4;



  if( id==0xFCD5 ) {
    data_format = warp_vCAEN;
    read_event = read_event_vCAEN;
    data_vCAEN_init();
  }



  if(io_debug) {
    printf("WarpIO: Data format = ");
    if(data_format == icarus)  printf("icarus\n");     
    if(data_format == mc_v1)   printf("mc old format\n");
    if(data_format == mc_v2)   printf("mc new format\n");
    if(data_format == warp_v3) printf("data_v3\n");
    if(data_format == warp_v4) printf("data_v4\n");
    if(data_format == warp_v5) printf("data_v5\n");
    if(data_format == warp_vCAEN) printf("data_vCAEN\n");
    if(data_format == warp_100_v1) printf("warp100_v1\n");
  }

  return 0;

}

int warp_close(void) {

//  if(data_format == icarus) icarus_end();

  if(io_is_compressed ) {
  //  if( (data_format == mc_v1) || (data_format == mc_v2) )   pclose(io_fp);
    //if(data_format == warp_vCAEN) warp_close_vCAEN();
     fclose(io_fp);
  } else {
	if(data_format!=warp_100_v1)
    		fclose(io_fp);
  }

  //if(data_format == warp_100_v1) warp_close_100_v1();
 // if(data_format == warp_v3) data_v3_end();
 // if(data_format == warp_v5) data_v5_end();

  return 0;

}
