#ifndef __WARP_LIB__
#define __WARP_LIB__

enum {icarus=0, mc_v1=1, warp_v1=2, warp_v3=3, warp_v4=4, warp_v5=5, mc_v2=6, warp_100_v1=7,warp_vCAEN=8};

typedef struct {
  int nrun;
  int npmt;
  int nsamples;
  int trig_delay;
  int baseline;
  int sample;
  int adc_phe;
  int single_rate;
} mc_head;

#ifndef __cplusplus

typedef struct {
  int     tm_sec;         /* seconds */
  int     tm_min;         /* minutes */
  int     tm_hour;        /* hours */
  int     tm_mday;        /* day of the month */
  int     tm_mon;         /* month */
  int     tm_year;        /* year */
  int     tm_wday;        /* day of the week */
  int     tm_yday;        /* day in the year */
  int     tm_isdst;       /* daylight saving time */
} tm ;

#endif

int  warp_gzip_init(void);
int warp_gzip_data(int nr, char* buffer, int no, char* out);
int warp_gzip_data_s(int nr, short* buffer, int no, short* out);
int  warp_gzip_end(void);
int  warp_gunzip_init(void);
int warp_gunzip_data(int nr, char* buffer, int no, char* out);
int warp_gunzip_data_s(int nb, short* p, int no, short* out);
int  warp_gunzip_end(void);

#ifdef _WARPLIB_

int io_debug = 0;
int io_is_compressed;
int io_run_number;
int io_number_of_pmt;
int io_number_of_samples;
int io_adc_full_scale;
int io_pretrigger =0;
bool io_is_caen = false;

FILE *io_fp;
FILE *io_fw;

//temporary to check card jitter
unsigned int cardtimes[20];

int warp_open(char*);
int warp_close(void);
int warp_wopen(int, char*);
int warp_wclose(void);
int write_head_mc(mc_head);
int read_head_mc(void);
int (* read_event)(int*, int*, int*, char*, int*, int*, int*, int*)=NULL;
int (* write_event)(int, int, int, int, int, int, unsigned int*)=NULL;

void io_set_debug(void);

#else

/* #ifdef __cplusplus */

/* extern "C" int io_debug; */
/* extern "C" int io_is_compressed; */
/* extern "C" int io_run_number; */
/* extern "C" int io_number_of_pmt; */
/* extern "C" int io_number_of_samples; */
/* extern "C" int io_adc_full_scale; */

/* extern "C" unsigned int cardtimes[20]; */

/* extern "C" FILE *io_fp; */
/* extern "C" FILE *io_fw; */

/* extern "C" int warp_open(char*); */
/* extern "C" int warp_close(void); */
/* extern "C" int warp_wopen(int, char*); */
/* extern "C" int warp_wclose(void); */
/* extern "C" int write_head_mc(mc_head); */
/* extern "C" int read_head_mc(void); */
/* extern "C" int (* read_event)(int*, int*, int*, char*, int*, int*, int*, int*); */
/* extern "C" int (* write_event)(int, int, int, int, int, int, unsigned int*); */

/* extern "C" void io_set_debug(void); */

/* #else */

extern int io_debug;
extern int io_is_compressed;
extern int io_run_number;
extern int io_number_of_pmt;
extern int io_number_of_samples;
extern int io_adc_full_scale;
extern int io_pretrigger;
extern bool io_is_caen;

//temporary to check card jitter
extern unsigned int cardtimes[20];

extern FILE *io_fp;
extern FILE *io_fw;

extern int warp_open(char*);
extern int warp_close(void);
extern int warp_wopen(int, char*);
extern int warp_wclose(void);
extern int write_head_mc(mc_head);
extern int read_head_mc(void);
extern int (* read_event)(int*, int*, int*, char*, int*, int*, int*, int*);
extern int (* write_event)(int, int, int, int, int, int, unsigned int*);

extern void io_set_debug(void);

/* #endif */

#endif
#endif


