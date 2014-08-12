#!/usr/bin/env bash

#
# do the mapnik-packaging stuff
#
make setup

#
# we're only using a test suite on Linux
#
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    npm install git+https://github.com/mapbox/mapbox-gl-test-suite.git
fi
