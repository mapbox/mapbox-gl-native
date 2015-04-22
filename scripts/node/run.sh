#!/usr/bin/env bash

set -e
set -o pipefail

BUILDTYPE=${BUILDTYPE:-Release}

source ./scripts/travis_helper.sh

# Add Mason to PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S

################################################################################
# X Server setup
################################################################################

if [[ ${OS} == 'linux' ]] ; then
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
fi

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

if [ ! -d node ] ; then
    mapbox_time "checkout_node_module" \
    git clone https://github.com/mapbox/node-mapbox-gl-native.git node
fi

pushd node
export MBGL=../

mapbox_time "install_module" \
npm install

mapbox_time "build_module" \
npm run-script build --build-from-source

################################################################################
# Test
################################################################################

mapbox_time "test_suite" \
make test
