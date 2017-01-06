#!/bin/bash

#ffplay -protocol_whitelist file,udp,rtp -i "$1"
ffplay -i "$1"
