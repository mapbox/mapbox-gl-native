#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Release TESTMUNK=yes

./android/scripts/common.sh $1
