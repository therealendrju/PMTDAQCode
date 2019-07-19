#include -lstdc++
CC=g++

LD=$(CC)

CFLAGS= -Wall -g -O2

#WARP_SOFTWARE=/home/warp/software/warp100

#
#

LIBWARP = -L../DataIO -lDataIO
INCWARP = -I../DataIO

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 
# -lMinuit2
ROOTGLIBS     = $(shell root-config --glibs)
# -lMinuit2

#MYSQLFLAGS    = $(shell mysql_config --cflags)

WARPID = $(shell uname -i)

SQLLIBS       =  -lnsl -lm -lstring
ifeq ($(WARPID),x86_64)
SQLLIBS       =  -lnsl -lm
endif

SRCS =   tools.cxx   wep.cxx
SRCS_100 = wep.cxx
 #fitter.cxx
OBJS = $(SRCS:%.cxx=%.o) 
OBJS_STAND = processor_code.o 
#OBJS = wep.o 
#OBJS_CAEN = processor_code_caen.o 


all: process average_sig raw_waveform_output  




wed.o:	wed.cxx
	$(CC) -c $(CFLAGS) $(ROOTCFLAGS) $<

wep.o:	wep.cxx
	$(CC) -c $(CFLAGS) -D __MY_SQL_TYPE__ $(INCWARP) $< -o $@

processor_code.o:	processor_code.cxx
	$(CC) -c $(CFLAGS) $(ROOTCFLAGS) $< -o $@


fitter.o:	fitter.cxx
	$(CC) -c $(CFLAGS) $(INCNR) $(ROOTCFLAGS) $< -o $@

tools.o:	tools.cxx
	$(CC) -c $(CFLAGS) $(INCNR) $(ROOTCFLAGS) $< -o $@

process:	$(OBJS_100) $(OBJS) $(OBJS_STAND) process.o
	$(CC) process.o $(OBJS) $(OBJS_100) $(OBJS_STAND) $(LIBWARP) -lz $(ROOTLIBS) $(LIBNR) $(SQLLIBS) -o $@


process.o:	process.cxx
	$(CC) -c $(CFLAGS) $(ROOTCFLAGS) $< -o $@


average_sig:	$(OBJS) $(OBJS_100) $(OBJS_STAND) average_sig.o
	$(CC) average_sig.o $(OBJS) $(OBJS_100) $(OBJS_STAND)  $(LIBWARP) -lz $(ROOTLIBS) $(LIBNR) $(SQLLIBS) -o $@

average_sig.o:	average_sig.cxx
	$(CC) -c $(CFLAGS) $(INCNR) $(ROOTCFLAGS) $< -o $@

raw_waveform_output:	$(OBJS) $(OBJS_100) $(OBJS_STAND) raw_waveform_output.o
	$(CC) raw_waveform_output.o $(OBJS) $(OBJS_100) $(OBJS_STAND)  $(LIBWARP) -lz $(ROOTLIBS) $(LIBNR) $(SQLLIBS) -o $@

raw_waveform_output.o:	raw_waveform_output.cxx
	$(CC) -c $(CFLAGS) $(INCNR) $(ROOTCFLAGS) $< -o $@

clean:
	rm -f *.o gui process average_sig  .depend


dep depend:
	$(CC) -E $(INCWARP) $(INCNR) $(ROOTCFLAGS) $(MYSQLFLAGS) -M $(SRCS) > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
