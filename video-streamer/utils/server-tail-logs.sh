#!/bin/bash
set -x

if [ "armv7l" = `uname -m` ]; then
    LOGFILE=/log/kern.log
else
    LOGFILE=/var/log/syslog
fi

tail -f "$LOGFILE"
