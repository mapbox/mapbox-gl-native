#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason" \
mapbox_time "install_mesa" \
mason install mesa 10.4.3
