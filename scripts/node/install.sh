#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason" \

if [ ${TRAVIS_OS_NAME} == "linux" ]; then
    mapbox_time "install_mesa" \
    mason install mesa 10.4.3
fi

mapbox_time $NODE_EXE \
mason install $NODE_EXE $NODE_VERSION

export PATH="`mason prefix $NODE_EXE $NODE_VERSION`/bin":"$PATH"

$NODE_EXE --version
npm --version
