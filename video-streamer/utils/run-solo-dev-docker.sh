#!/bin/bash

CURDIR=$(pwd)
WORKSPACE="$(dirname "$CURDIR")"
IMAGE=solo-dev


XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth

touch $XAUTH
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

docker run -it \
       --volume=$XSOCK:$XSOCK:rw \
       --volume=$XAUTH:$XAUTH:rw \
       --volume="$WORKSPACE":/root/workspace:rw \
       --env="XAUTHORITY=${XAUTH}" \
       --env="DISPLAY" \
       "$IMAGE"



