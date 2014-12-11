#!/bin/bash

echo "check_proc.sh" >> /home/blue/2015/log/log.log

ps -e | grep rpoldaemon | grep -v grep
