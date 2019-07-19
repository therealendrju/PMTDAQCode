
#CC=g++

#LD=$(CC)

#CPPFLAGS= -Wall -g -O2 

CC           = g++
CPPFLAGS      = -O -g -Wall -fPIC 
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared

# Library definition

LIBS=

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

#MYSQLFLAGS    = $(shell mysql_config --cflags)

#INCWARP = -I/home/warp/warp100/DataIO -I/home/warp/warp100/mcrbee10
#LIBWARP = -L/home/warp/warp100/DataIO -lDataIO -Wl,-rpath,/home/warp/warp100/DataIO

INCWARP = -I$(PWD)/../DataIO  -I$(PWD)/../mcrbee10
LIBWARP = -L$(PWD)/../DataIO -lDataIO -Wl,-rpath,$(PWD)/../DataIO
LIBWARP += -Wl,-rpath,$(PWD)/../DataIO

#INCWARP = -I/home/Andrzej/code/software/include
#LIBWARP = -L/home/Andrzej/code/software/lib -lDataIO -L/usr/local/lib 

#INCNR = -I/home/warp/software/NR/other
#LIBNR = -L/home/warp/software/NR/recipes -lNR


# LD            = g++
# LDFLAGS       = -g
# SOFLAGS       = -shared

CPPFLAGS     += $(ROOTCFLAGS)
CPPFLAGS     += $(INCWARP)
CPPFLAGS     += $(MYSQLFLAGS) 
LIBS         += $(ROOTLIBS) 
GLIBS         = $(ROOTGLIBS) 

WARPID = $(shell uname -i)

SQLLIBS       =  -lnsl -lm 
ifeq ($(WARPID),x86_64)
SQLLIBS       = -lnsl -lm 
endif
SRCS = PMTwidget.cxx maingui.cxx MainFrame.cxx AuxFrame.cxx  channel.cxx  

OSRCS =  ../mcrbee10/tools.cxx ../mcrbee10/processor_code.cxx  

OSRCS_100 = ../mcrbee10/wep.cxx process.cxx

DSRCS = GUIwrapper.cxx GUIwrapperCint.cxx 

OBJS = $(SRCS:%.cxx=%.o)

DOBJS = $(DSRCS:%.cxx=%.o)

OOBJS= $(OSRCS:%.cxx=%.o)
OOBJS_100= $(OSRCS_100:%.cxx=%.o)

#RLIB =  libEvtestn.so libGUItestn.so

all:	newser

newser:$(OBJS)  $(DOBJS) $(OOBJS) $(OOBJS_100)
	$(LD) -o $@ $(OBJS) $(DOBJS) $(OOBJS) $(OOBJS_100) $(LIBWARP) -lz $(SQLLIBS) $(GLIBS) -lstdc++ 


%.o: %.cxx
	$(CC) $(CPPFLAGS) -c $< -o $@

../mcrbee10/wep.o:	../mcrbee10/wep.cxx
	cd ../mcrbee10;make wep.o;cd -;

#	$(CC) -o ../luciano/wep.o -c $(CFLAGS) -D __SQL_TYPE__ $(INCWARP) $<


process.o:	process.cxx
	$(CC) $(CPPFLAGS) -D __MY_SQL_TYPE__ -c $< -o $@

GUIwrapperCint.cxx: GUIwrapper.h GUIwrapperLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint  -f  $@ -c  $^



#libGUItestn.so:  GUItest.C GUItest.h
#	make -f MakeGUItest

dep depend:
	$(CC) -E $(CPPFLAGS) -M $(SRCS) $(DSRCS) maingui.cxx > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif


#install:
#	cp newser /home/warp/analysis/input/
#	cp lib*so /home/warp/analysis/input/

clean:
	rm -f *.o *Cint* newser $OOBJS	
