#!/bin/csh

set id = `ps -e | grep rpoldaemon | grep $USER | grep -v grep | awk '{ print $1}'`

if ( $id != "" ) then
    exit 0 
else
    exit 1
endif
