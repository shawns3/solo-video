# solo-video

Video Pipeline Tools for the 3DR SOLO Quadcopter

# running onboard SOLO

>> edit config.h
```shell
cd video-streamer
make
./video-streamer
```

# testing video pipeline on laptop using docker

>> put a video clip in test-images/
>> edit config.h
>> make sure you have docker installed
cd video-streamer/docker
make
cd ..
./utils/run-solo-dev-docker.sh

>> in docker container
cd video-streamer
make
