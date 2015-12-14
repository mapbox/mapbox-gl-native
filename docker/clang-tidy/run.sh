#!/usr/bin/env bash

set -e
set -o pipefail

./docker/build.sh

docker build -t mapbox/gl-native:clang-tidy docker/clang-tidy

docker run \
    -i \
    -v `pwd`:/home/mapbox/build \
    -t mapbox/gl-native:clang-tidy \
    build/docker/clang-tidy/tidy.sh
