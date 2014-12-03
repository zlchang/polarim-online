#!/bin/bash
#
#	Set variables for RHIC pC and HJET polarimeters
#	I.Alekseev & D.Svirida 2014
#	decode ring and set 2015 run environment
#	Should be sourced as ". polsetvar.sh" from bash script

ME=`hostname`
YEAR=2015

case $ME in
    acnlinf9.* ) POLRING="blue"    ;;
    acnlinf8.* ) POLRING="yellow"  ;;
    acnlinb3.* ) POLRING="hjet"    ;;
    acnlinc6.* ) POLRING="reserve" ;;
    *          ) POLRING="reserve" ;;
esac

POLARIM=/home/polarim
POLBIN=$POLARIM/polarim/bin
POLCONF=$POLARIM/polarim/config
POLSTORE=$POLARIM/$POLRING/$YEAR

export POLRING POLARIM POLBIN POLSTORE POLCONF
