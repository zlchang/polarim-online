#!/bin/bash
# Analize(reanalize) jet run

. polsetvar.sh

RUN=$1

export HJET_ROOTFILE="$POLSTORE/root/"$RUN".root"
export HJET_DATAFILE="$POLSTORE/"$RUN".data"
export HJET_DAQ=""

#export HJET_CALIB="../config/calib_${2:-1103.605}.dat"
#export HJET_CALIB="../config/calib_${2:-3003}.dat" #Hiromi changed t0=t0+4nsec
#export HJET_CALIB="../config/calib_${2:-1602}.dat"
#export HJET_CALIB="../config/calib_${2:-1802}.dat"
#export HJET_CALIB="../config/calib_${2:-2002}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1103.605}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1103.606}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1103.616}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-2603.602}.dat" # revert to 1103.605 3/27/09
#export HJET_CALIB="$CONFIG/calib_${2:-2703.601}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-2703.612}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-2703.622}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-0902.705}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1002.706}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1002.707}.dat"
#export HJET_CALIB="$CONFIG/calib_${2:-1102.706}.dat" # Run11 250 GeV
#export HJET_CALIB="$CONFIG/calib_${2:-1404.709}.dat"  # Run11  24 GeV
#export HJET_CALIB="$CONFIG/calib_${2:-0602.801}.dat"  # Run12
#export HJET_CALIB="$CONFIG/calib_${2:-0802.807}.dat"  # Run12
#export HJET_CALIB="$CONFIG/calib_${2:-3101.802}.dat"  # Run12
#export HJET_CALIB="$CONFIG/calib_${2:-0703.902}.dat"  # Run13
export HJET_CALIB="$POLCON/calib_${2:-1403.904}.dat"  # Run13
if [ "x"$1 == "x" ] ; then 
    echo "Usage: ./jetanal.sh <RUN> [<CALIB>]"
else
# to run in background (no displays) use -b
#    root -b jetrun.C 
# to run in forground (display during analysis) use -l
    root -l jetrun.C
fi
