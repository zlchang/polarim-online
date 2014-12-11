#!/bin/bash

echo "check.sh" >> /home/blue/2015/log/log.log

i_d=`ps -e | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $2}'`

if [ "x$i_d" != "x" ] ; then
    exit 0 
else
    exit 1
fi

