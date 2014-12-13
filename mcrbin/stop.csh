#!/bin/csh

set id = `ps -e -u $USER | grep rpoldaemon | awk '{ print $1}'`
if ( "$id" != "" ) then
    kill -s TERM $id
    echo "rpoldaemon stopped!"
    exit 0
else
    echo "rpoldaemon is not running!"
    exit 1
endif
