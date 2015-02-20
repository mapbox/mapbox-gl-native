#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Release

./platform/android/scripts/common.sh $1
