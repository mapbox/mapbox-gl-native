#!/usr/bin/env bash

set -e
set -o pipefail

# Mason exists on PATH from sourcing mbgl install script
mapbox_time "node" \
mason install node $NODE_VERSION

export PATH="`mason prefix node $NODE_VERSION`/bin":"$PATH"

node --version
npm --version
