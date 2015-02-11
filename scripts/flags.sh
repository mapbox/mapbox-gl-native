#!/usr/bin/env bash

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    export DISPLAY=:99.0

    if [[ ${MASON_PLATFORM} != "android" ]]; then
        sh -e /etc/init.d/xvfb start
    fi

    # use g++ that supports c++11
    if [[ ${CXX} == "g++" ]]; then
        export CXX="g++-4.8"
        export CC="gcc-4.8"
    fi

    # If building in debug then turn on sanitizers.
    # They should both work with latest g++/clang++
    # but it's not worth a standalone matrix so here
    # we just test address->gcc and thread->clang for
    # some limited coverage
    # if [[ ${BUILDTYPE} == "Debug" ]]; then
    #     if [[ ${CXX} == "g++" ]]; then
    #         export CXXFLAGS="-fsanitize=address -g ${CXXFLAGS}"
    #         export CFLAGS="-fsanitize=address -g ${CFLAGS}"
    #         export LDFLAGS="-fsanitize=address  ${LDFLAGS}"
    #     elif [[ ${CXX} == "clang++" ]]; then
    #         export CXXFLAGS="-fsanitize=thread -g -fPIC ${CXXFLAGS}"
    #         export CFLAGS="-fsanitize=thread -g ${CFLAGS}"
    #         export LDFLAGS="-fsanitize=thread -pie ${LDFLAGS}"
    #     fi
    # fi
fi
