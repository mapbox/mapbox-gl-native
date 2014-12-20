#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Debug TESTMUNK=no

./common.sh $1
