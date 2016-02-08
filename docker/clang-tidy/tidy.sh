#!/usr/bin/env bash

# set -e
# set -o pipefail

export FLAVOR=linux
export CXX=clang++-3.8
export BUILDTYPE=Release

cd build

# before_install
source ./scripts/travis_helper.sh

# install
./platform/${FLAVOR}/scripts/install.sh

export CLANG_TIDY=clang-tidy-3.8
make tidy
