#!/bin/csh

echo "check.csh" >> /home/blue/2015/log/log.log

set id = `ps -e | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $2}'`

if ( $id != "" ) then
    exit 0 
else
    exit 1
endif
