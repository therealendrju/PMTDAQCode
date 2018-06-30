#ifndef __MY_DATA__
#define __MY_DATA__


typedef struct {
  unsigned short header_size;
  unsigned short header_version;
  unsigned int event_number;
  unsigned int event_time;
  unsigned int event_size;
} event_header;

//extern int eh_size;
//extern event_header eh;

typedef struct {
  unsigned short    header_size;
  unsigned short    header_version;
} run_hh;

typedef struct {
  unsigned int      run_number;
  unsigned int      date_time_start;
  unsigned int      date_time_stop;
  unsigned short    daq_version;
  unsigned short    hw_setup_id;
  unsigned int      number_of_pmt;
  unsigned int      number_of_samples;
  unsigned int      number_of_events;
} run_header1;

typedef struct {
  unsigned int      run_number;
  unsigned int      date_time_start;
  unsigned int      date_time_stop;
  unsigned short    daq_version;
  unsigned short    hw_setup_id;
  unsigned short    number_of_pmt;
  unsigned short    full_scale;
  unsigned int      number_of_samples;
  unsigned int      number_of_events;
  short             dc_offset;
  short             trig_delay;
} run_header2;


typedef struct {
  unsigned short    header_size;
  unsigned short    header_version;
  unsigned long     run_number;
  unsigned long     date_time_start;
  unsigned long     date_time_stop;
  unsigned short    daq_version;
  unsigned short    hw_setup_id;
  unsigned short    number_of_pmt;
  unsigned short    full_scale;
  unsigned long     number_of_samples;
  unsigned long     number_of_events;
  short             dc_offset;
  short             trig_delay;
} run_header3;

typedef struct {
  unsigned short header_size;
  unsigned short header_version;
  unsigned long event_number;
  unsigned long event_time;
  unsigned long event_size;
} event_header2;






#endif
