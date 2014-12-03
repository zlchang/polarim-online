#!/bin/bash

#source /usr/local/cern/root/bin/thisroot.sh

#export CERNLIB=/usr/local/cern/2006b
#export CDEV=/usr/local/cdev
#export CNIPOL=/usr/local/cnipol

#export CDEVDDL=${CNIPOL}/online/cdev/PolarClient.ddl

#export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
#export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov
#export CDEVSHOBJ=${CDEV}/lib
#export LD_LIBRARY_PATH=${CDEVSHOBJ}:${LD_LIBRARY_PATH}
#export PATH=${CERNLIB}/bin:${PATH}

. polsetvar.sh
LOGDIR=$POLSTORE/log

case $POLRING in
    blue   ) RING="B" ;;
    yellow ) RING="Y" ;;
    *      ) echo "Wrong ring !!!"; exit     ;;
esac

${POLBIN}/rpoldaemon $RING -l ${LOGDIR}/rpoldaemon${1}.log -r ${POLBIN}/rpolmeasure.sh 
