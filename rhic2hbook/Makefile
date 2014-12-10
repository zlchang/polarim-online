### PROGS = rhic2hbook
### MPROGS = onliplot.kumac pvector.kumac pvector.pl lr_spinpat.pl online_polar.pl rampplot.kumac repol.sh repol.kumac
###
### include ../include/common.mk
### 
### install::
###	@if [ ! -f $(CNIPOL_DIR)/bin/analyze.sh ]; then ln -s `pwd`/test.sh $(CNIPOL_DIR)/bin/analyze.sh; fi
###
### CERNLIB_DIR=/usr/local/cern
### INC_PATHS += -I$(CERNLIB_DIR)/include/cfortran
### LIB_PATHS += -L$(CERNLIB_DIR)/lib

#CXXFLAGS += $(shell root-config --cflags)
#CFLAGS += $(shell root-config --cflags)
#LIBS += $(shell root-config --ldflags --libs)

### rhic2hbook : rhic2hbook.o rdatautilp.o rdatautil.o
###	$(FC) $^ -Wno-globals -m32 -o $@ $(LIB_PATHS) -lpacklib -lkernlib -lmathlib -lcdev -lm -ldl -lstdc++ $(LIBS)

include /vobs/libs/makefiles/MakeStd.inc

NAME = rhic2hbook

USESOLIBS = yes
INC = -I../include -I/usr/include/cernlib/2006/cfortran
USER_FFLAGS = -m32
USER_CXXFLAGS = $(shell root-config --cflags)
LDFLAGS = -m32 -L/usr/lib/cernlib/2006-g77/lib $(shell root-config --libs)

PROG1 = rhic2hbook
SRCS1 = rhic2hbook.f rdatautilp.cpp rdatautil.cpp
LIBS1 = cdevCns cdev includeCdev GenData rhicError Async setHist
LD1 = g77
#SYSLIBS1 = packlib kernlib mathlib  gfortranbegin gfortran g2c
SYSLIBS1 = packlib kernlib mathlib

include $(MAKEDIR)/MakeApp.inc
