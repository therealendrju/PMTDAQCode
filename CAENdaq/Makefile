
CC=gcc

LD=$(CC)

CFLAGS= -Wall -g -O2 -fPIC  -D_LINUX -I../DataIO 
#CFLAGS+=-D_WARP_USE_SHM_=0 

LIBS=  -lpthread -lz  -lm -lCAENDigitizer -lCAENComm -lCAENVME -L../DataIO -lDataIO 

SRCS = daq_main.c daq_pci.c daq_writer.c gziplib.c
OBJS = $(SRCS:%.c=%.o)

all:	daq

daq:	$(OBJS)
	$(LD) -o daq $(OBJS) $(LIBS) 

daq_main.o:	daq_main.c
	$(CC) -c $(CFLAGS) daq_main.c

daq_pci.o:	daq_pci.c daq_pci.h
	$(CC) -c $(CFLAGS) daq_pci.c

daq_writer.o:	daq_writer.c
	$(CC) -c $(CFLAGS) daq_writer.c

.cc.o:
	$(CC) -c $(CFLAGS) $<

clean:
	@rm -f $(OBJS) daq
