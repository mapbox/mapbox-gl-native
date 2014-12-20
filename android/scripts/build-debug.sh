#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Debug TESTMUNK=no

./android/scripts/common.sh $1
