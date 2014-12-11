#!/bin/csh

echo "stop.csh" >> /home/blue/2015/log/log.log

set id = `ps -e -u $USER | grep rpoldaemon | awk '{ print $1}'`
kill -s TERM $id
echo "rpoldaemon stopped!"
exit $?
