# Custom 3DR Solo Quadcopter Video Pipeline

This code is a work in progress. Your mileage may vary.

[This forum post](https://discuss.dronekit.io/t/solo-video-acquire-please/320)
was very helpful during development.

This project has video pipeline tools for executing custom video
processing onboard the 3DR SOLO quadcopter. Currently, this replaces
the default onboard video streaming service with a custom service that
is compatible with the original service and additionally implements a
custom image processing routine. The current routine performs face
detection utilizing the Haar Cascades detector implemented in OpenCV.


# Testing the Code

I used a docker image on my laptop to facilitate rapid code
development and testing. The image has OpenCV and the 0.1 gstreamer
libs installed on it.

## Setup and Configuration

- Install docker on your computer
- Put a mp4/H264 video clip in test-images/. I used the video clip "matrix.mp4".
- Edit the config.h file for your setup

## Build and Execution

``` sh
cd video-streamer/docker
make
cd ..
./utils/run-solo-dev-docker.sh
```

Then, from the docker container:
``` sh
cd video-streamer
make
./video-streamer
```


# Running Onboard

## Setup and Configuration

- TODO: I need to update these setup instructions as they are not complete.
- Stop the original video streaming service
  - comment out the VIDLAUNCH line in inittab
  - reboot
- Install the needed packages onboard.
``` sh
smart channel --add cortexa9hf_vfp_neon type=rpm-md baseurl=http://downloads.yoctoproject.org/releases/yocto/yocto-1.5.1/rpm/cortexa9hf_vfp_neon/ -y
smart channel --add cortexa9hf_vfp_neon_mx6 type=rpm-md baseurl=http://downloads.yoctoproject.org/releases/yocto/yocto-1.5.1/rpm/cortexa9hf_vfp_neon_mx6/ -y
smart update
smart install gcc gcc-symlinks libc6-dev gcc-dev binutils python-dev -y
```
- Edit the config.h file for your setup

## Build and Execution

``` sh
cd video-streamer
make
./video-streamer
```
