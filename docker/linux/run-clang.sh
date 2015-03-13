#!/usr/bin/env bash

set -e
set -o pipefail

docker build \
    -t mapbox/gl-native:linux \
    docker/linux

docker run \
    -i \
    -e "CXX=clang++" \
    -v `pwd`:/home/mapbox/build \
    -t mapbox/gl-native:linux \
    build/docker/linux/test.sh
