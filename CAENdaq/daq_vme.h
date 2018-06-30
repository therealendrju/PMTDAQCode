
#include "CAENVMElib.h"

typedef struct {
  unsigned event_size    : 28;
  unsigned fixed         :  4;
  unsigned channel_mask  :  8;
  unsigned reserved_1    : 19;
  unsigned board_id      :  5;
  unsigned event_counter : 24;
  unsigned reserved_2    :  8;
  unsigned trig_time_tag : 32;
} vme_ev_head;

long          BHandle;
const ulong   BaseAddr =   0x32100000;

