# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

#SOFTWARE_LOC = /home/andrzej/software/warp100

TARGET = libDataIO.so
MAJOR=1
REVISION = 2.8

# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
CXXFLAGS      = -O2 -g -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared -Wl,-soname,$(TARGET).$(MAJOR)

WARPID := $(shell uname -i)
ifeq ($(WARPID),x86_64)
CXXFLAGS     += -m64
endif

#------------------------------------------------------------------------------
SRCS = warplib.c  gziplib.c  data_vCAEN.c 
OBJS:=$(SRCS:%.c=%.o)
#------------------------------------------------------------------------------

all:            $(TARGET)

$(TARGET):	$(OBJS)
		@echo "Building $@"
		$(LD) $(SOFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET).$(MAJOR).$(REVISION) -lz 
		@echo "$@ done"
		@rm -f libDataIO
		ar q libDataIO.a $(OBJS) 
		ranlib libDataIO.a

#install:
#		@echo "removing old files in" $(WARP_SOFTWARE)
#		@rm -f $(WARP_SOFTWARE)/include/libWarpIO.h
#		@rm -f $(WARP_SOFTWARE)/lib/$(TARGET).*
#		@rm -f $(WARP_SOFTWARE)/lib/libWarpIO.a
#		@echo -n "installing new ones...."
#		@cp libWarpIO.h $(WARP_SOFTWARE)/include
#		@cp $(TARGET).$(MAJOR).$(REVISION) $(WARP_SOFTWARE)/lib
#		@cp  libWarpIO.a $(WARP_SOFTWARE)/lib
#		@(cd $(WARP_SOFTWARE)/lib; ln -sf $(TARGET).$(MAJOR).$(REVISION) $(TARGET))
#		@(cd $(WARP_SOFTWARE)/lib; ln -sf $(TARGET).$(MAJOR).$(REVISION) $(TARGET).$(MAJOR))
#		@echo "done !"


.c.o:		libDataIO.h
		$(CXX) $(CXXFLAGS) -c $<

clean:
		@rm -f libDataIO.so* libDataIO.a .depend $(OBJS) 

dep depend:
	$(CXX) $(CXXFLAGS) -M $(SRCS) > .depend

ifeq (.dep,$(wildcard .dep))
include .dep
endif

