#!/bin/csh

set id = `ps -e | grep rpoldaemon | grep -v grep | awk '{ print $1}'`

if ( "$id" != "" ) then
    echo "rpoldaemon is running already !"
    exit 1
endif

source /usr/public/ENV/setup-cdev
source /usr/local/src/root/bin/thisroot.csh /usr/local/src/root/

cd /home/polarim/bin
./rpoldaemon.sh &

exit 0
