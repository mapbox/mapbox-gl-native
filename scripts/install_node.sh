#!/usr/bin/env bash

set -e
set -o pipefail

mason install node $NODE_VERSION
export PATH="`mason prefix node $NODE_VERSION`/bin":"$PATH"
node --version
npm --version
