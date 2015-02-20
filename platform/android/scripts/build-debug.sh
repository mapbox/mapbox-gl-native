#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Debug

./platform/android/scripts/common.sh $1
