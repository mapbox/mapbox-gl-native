#!/usr/bin/env bash

set -e
set -o pipefail

source ./platform/linux/scripts/setup.sh

################################################################################
# Coveralls
################################################################################

mapbox_time "make_coveralls" \
make coveralls -j${JOBS}
