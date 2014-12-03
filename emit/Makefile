#PROGS = emitscan
#include ../include/common.mk
#emitscan: emitscan.o emitsend.o ../rpoldaemon/globals.o ../rpoldaemon/rpoldata.o


include /vobs/libs/makefiles/MakeStd.inc

NAME = emitscan

USESOLIBS = yes
INC = -I../include
USER_CXXFLAGS = $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs)

PROG1 = emitscan
SRCS1 = emitscan.cpp emitsend.cpp globals.cpp rpoldata.cpp
LIBS1 = cdevCns cdev includeCdev GenData rhicError Async setHist
PRIVATE_HEADERS1 = emitdefn.h

include $(MAKEDIR)/MakeApp.inc
