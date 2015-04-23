#!/usr/bin/env bash

set -e
set -o pipefail

export TRAVIS_OS_NAME=linux

cd build

source ./scripts/local_mason.sh
mason install mesa 10.4.3
export LD_LIBRARY_PATH=`mason prefix mesa 10.4.3`/lib

export DISPLAY=:99.0
Xvfb :99 -ac -screen 0 1024x768x24 &

if [[ ${CXX} == "g++" ]]; then
    export CXX="g++-4.8"
    export CC="gcc-4.8"
fi

source ./scripts/install_node.sh

make linux -j`nproc`
make test-* -j`nproc`
./scripts/compare_images.sh
