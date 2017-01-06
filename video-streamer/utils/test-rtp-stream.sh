#!/bin/bash
set -x
gst-launch-0.10 mfw_v4lsrc device=/dev/video0 name=src ! mfw_ipucsc ! video/x-raw-yuv, format=\(fourcc\)I420, width=\(int\)1280, height=\(int\)720, framerate=\(fraction\)30/1, pixel-aspect-ratio=\(fraction\)1/1 ! queue ! vpuenc codec=6 bitrate=1800000 gopsize=15 force-framerate=true framerate-nu=25 name=enc ! rtph264pay config-interval=1 pt=96 name=rtp ! udpsink host=10.1.1.1 port=5550
