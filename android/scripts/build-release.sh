#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Release

./android/scripts/common.sh $1
