#!/bin/bash
set -x
kill -s INT `cat /tmp/video-streamer-pid`
