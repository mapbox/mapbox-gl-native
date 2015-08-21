#!/usr/bin/env bash

set -e
set -o pipefail

# Mason exists on PATH from sourcing mbgl install script
mapbox_time $NODE_EXE \
mason install $NODE_EXE $NODE_VERSION

export PATH="`mason prefix $NODE_EXE $NODE_VERSION`/bin":"$PATH"

$NODE_EXE --version
npm --version
