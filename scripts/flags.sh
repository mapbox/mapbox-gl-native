#!/usr/bin/env bash

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    export DISPLAY=:99.0
    sh -e /etc/init.d/xvfb start

    # use g++ that supports c++11
    if [[ ${CXX} == "g++" ]]; then
        export CXX="g++-4.8" ;
    fi

    # If building in debug then turn on sanitizers.
    # They should both work with latest g++/clang++
    # but it's not worth a standalone matrix so here
    # we just test address->gcc and thread->clang for
    # some limited coverage
    if [[ ${BUILDTYPE} == "Debug" ]]; then
        if [[ ${CXX} == "g++" ]]; then
            export CXXFLAGS="-fsanitize=address ${CXXFLAGS}"
            export CFLAGS="-fsanitize=address ${CFLAGS}"
            export LDFLAGS="-fsanitize=address  ${LDFLAGS}"
        elif [[ ${CXX} == "clang++" ]]; then
            export CXXFLAGS="-fsanitize=thread -fPIC ${CXXFLAGS}"
            export CFLAGS="-fsanitize=thread ${CFLAGS}"
            export LDFLAGS="-fsanitize=thread -pie ${LDFLAGS}"
        fi
    fi
fi

# turn off a few warnings
export CXXFLAGS="-Wno-unknown-warning-option -Wno-unused-local-typedefs -Wno-unknown-pragmas ${CXXFLAGS}";
