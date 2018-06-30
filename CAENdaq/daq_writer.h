
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
} run_header;

typedef struct {
  unsigned short header_size;
  unsigned short header_version;
  unsigned long event_number;
  unsigned long event_time;
  unsigned long event_size;
} event_header;

typedef struct {
  unsigned long channel;
  unsigned long data_size;
  unsigned long timestamp1;
  unsigned long timestamp2;
} acqiris_pmt_head;

#define RUN_HEADER_SIZE  16  // in longwords
#define EV_HEADER_SIZE   8  // in longwords
#define PMT_HEADER_SIZE  4  // in longwords

