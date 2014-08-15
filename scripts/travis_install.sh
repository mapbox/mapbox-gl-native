#!/usr/bin/env bash

#
# do the mapnik-packaging stuff
#
make setup

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # we're need the test suite on Linux
    #
    npm install git+https://github.com/mapbox/mapbox-gl-test-suite.git
fi
