#!/usr/bin/env bash

set -e
set -o pipefail

export FLAVOR=linux
export BUILDTYPE=Release
export CXX=g++-4.9

git submodule update --init .mason
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

mason install mesa 10.4.3

git submodule update --init styles

make linux -j`nproc` BUILDTYPE=${BUILDTYPE}
