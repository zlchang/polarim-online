#!/bin/bash

echo "========================= $USER start.csh =========================================" >> /home/blue/2015/log/log.log
hostname  >> /home/blue/2015/log/log.log
date  >> /home/blue/2015/log/log.log
set >> /home/blue/2015/log/log.log

cd /home/polarim/bin
./rpoldaemon.sh >>/dev/null &

exit $?
