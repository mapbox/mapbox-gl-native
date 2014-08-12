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
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # let's pretty it up on OS X/iOS
    #
    gem install xcpretty --no-rdoc --no-ri --no-document --quiet
fi
