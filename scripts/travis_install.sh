#!/usr/bin/env bash

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # we'll need the test suite on Linux
    #
    npm install git+https://github.com/mapbox/mapbox-gl-test-suite.git
fi
