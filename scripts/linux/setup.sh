#!/usr/bin/env bash
# This script is sourced; do not set -e or -o pipefail here.

# Ensure mason is on the PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S

################################################################################
# X Server setup
################################################################################

# Start the mock X server
if [ -f /etc/init.d/xvfb ] ; then
    mapbox_time "start_xvfb" \
    sh -e /etc/init.d/xvfb start
    sleep 2 # sometimes, xvfb takes some time to start up
fi

# Make sure we're connecting to xvfb
export DISPLAY=:99.0

# Make sure we're loading the 10.4.3 libs we installed manually
export LD_LIBRARY_PATH="`mason prefix mesa 10.4.3`/lib:${LD_LIBRARY_PATH:-}"

mapbox_time "glxinfo" \
glxinfo
