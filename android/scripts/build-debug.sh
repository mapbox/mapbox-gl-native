#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Debug

./android/scripts/common.sh $1
