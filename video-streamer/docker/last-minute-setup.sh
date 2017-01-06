#!/bin/bash

ln -s /dev/null /dev/raw1394  # OpenCV gets grumpy without this; this cannot be part of the Dockerfile because /dev/ is recreated on start
service rsyslog start  # a logger is not started by default

cd /root/workspace
bash
