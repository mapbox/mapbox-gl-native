#!/usr/bin/env bash

# set -e
# set -o pipefail

export _CXX=g++-5
export _CC=gcc-5
export BUILDTYPE=Release

# Node
. ~/.nvm/nvm.sh
nvm use 4.4.5

# Xvfb
start-stop-daemon --start --pidfile ~/xvfb.pid --make-pidfile --background --exec /usr/bin/Xvfb -- :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset

cd build

# before_install
source ./scripts/travis_helper.sh
source ./scripts/travis_setup.sh

make linux
make test
