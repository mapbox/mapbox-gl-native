#!/usr/bin/env bash

# set -e
# set -o pipefail

export FLAVOR=linux
export CXX=g++-4.9
export BUILDTYPE=Release

# Node
. ~/.nvm/nvm.sh
nvm use 0.10

# Xvfb
start-stop-daemon --start --pidfile ~/xvfb.pid --make-pidfile --background --exec /usr/bin/Xvfb -- :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset

cd build

# before_install
source ./scripts/travis_helper.sh

# install
./scripts/${FLAVOR}/install.sh

# script
./scripts/${FLAVOR}/run.sh
