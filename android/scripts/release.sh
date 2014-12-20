#!/usr/bin/env bash

set -e
set -o pipefail

export BUILDTYPE=Release TESTMUNK=yes

./common.sh $1
