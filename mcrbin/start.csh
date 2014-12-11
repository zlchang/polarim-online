#!/bin/csh 

echo "========================= $USER start.csh =========================================" >> /home/blue/2015/log/log.log
hostname  >> /home/blue/2015/log/log.log
date  >> /home/blue/2015/log/log.log
setenv > /home/blue/2015/log/log.log

cd /home/polarim/bin
./rpoldaemon.sh &

exit $?
