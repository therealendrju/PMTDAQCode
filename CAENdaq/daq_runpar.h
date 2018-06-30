
typedef struct {
  unsigned long number_of_pmt;
  unsigned long number_of_samples;
  unsigned long events_triggered;
  unsigned long events_readout;
  unsigned long events_written;
  unsigned long event_size_32;
  unsigned short number_of_buffers;
  unsigned short full_scale;
  short          dc_offset;
  short          trig_delay;
  short 	trig_level;
  short 	trig_source;
} daq_control_t;

typedef struct {
  unsigned long run_number;
  unsigned long events_per_run;
  unsigned long events_per_file;
  unsigned long number_of_runs;
  unsigned short enable_gzip;
  unsigned short terminate;
} run_control_t;

#ifdef _DAQ_MAIN_

daq_control_t daq_control;
run_control_t run_control;

#else

extern daq_control_t daq_control;
extern run_control_t run_control;

#endif

