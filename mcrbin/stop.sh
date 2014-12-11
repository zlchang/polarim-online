#!/bin/bash

echo "stop.sh" >> /home/blue/2015/log/log.log

i_d=`ps -e -u $USER | grep rpoldaemon | awk '{ print $1}'`
kill -s TERM $i_d
echo "rpoldaemon stopped!"
exit $?
