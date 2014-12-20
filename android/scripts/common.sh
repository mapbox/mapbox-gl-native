#!/usr/bin/env bash

set -e
set -o pipefail

NAME=$1

export MASON_PLATFORM=android

make android -j$JOBS BUILDTYPE=${BUILDTYPE}
