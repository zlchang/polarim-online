### PROGS = rpoldaemon cnidaemon
### IPROGS = rpoldaemon.sh rpolmeasure.sh failed.gif
### include ../include/common.mk
### rpoldaemon: rpoldaemon.o globals.o rpoldata.o
### cnidaemon: cnidaemon.o globals.o rpoldata.o

include /vobs/libs/makefiles/MakeStd.inc

NAME = rhicpol

USESOLIBS = yes
INC = -I../include
USER_CXXFLAGS = $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs)

PROG1 = rpoldaemon
SRCS1 = rpoldaemon.cpp globals.cpp rpoldata.cpp
LIBS1 = cdevCns cdev includeCdev GenData rhicError Async setHist
PROG2 = cnidaemon
SRCS2 = cnidaemon.cpp globals.cpp rpoldata.cpp
LIBS2 = cdevCns cdev includeCdev GenData rhicError Async setHist

include $(MAKEDIR)/MakeApp.inc
