#!/usr/bin/env bash

set -e
set -o pipefail

# enable testing locally (not on travis)
if [[ "${TRAVIS_OS_NAME:-false}" == false ]]; then
    if [[ $(uname -s) == "Darwin" ]]; then
        TRAVIS_OS_NAME="osx"
    elif [[ $(uname -s) == "Linux" ]]; then
        TRAVIS_OS_NAME="linux"
    fi
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

    if [[ `lsb_release -r` =~ "12.04" ]]; then
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    fi
    sudo apt-get update -y
    sudo apt-get -y install lib32stdc++6 lib32z1
    sudo apt-get -y install gcc-4.8 g++-4.8
    sudo apt-get -y install git build-essential zlib1g-dev automake libtool xutils-dev make cmake pkg-config
    sudo apt-get -y install libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev

    wget --header "Cookie: gpw_e24=http%3A%2F%2Fwww.oracle.com%2F; oraclelicense=accept-securebackup-cookie" http://download.oracle.com/otn-pub/java/jdk/7u60-b19/jdk-7u60-linux-x64.tar.gz
    tar -xzf jdk-7u60-linux-x64.tar.gz
    export JAVA_HOME=$PWD/jdk1.7.0_60
    export PATH=$JAVA_HOME/bin:$PATH

    wget http://www.us.apache.org/dist/ant/binaries/apache-ant-1.9.4-bin.tar.bz2
    tar -xjf apache-ant-1.9.4-bin.tar.bz2
    export ANT_HOME=$PWD/apache-ant-1.9.4
    export PATH=$ANT_HOME/bin:$PATH

    wget http://dl.google.com/android/android-sdk_r23-linux.tgz
    tar -xzf android-sdk_r23-linux.tgz
    export ANDROID_HOME=$PWD/android-sdk-linux
    export PATH=$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH
    echo -e "yes\nyes\n" | android update sdk -u -a -t tools,platform-tools,android-19,build-tools-19.1.0,extra-android-support

    wget http://dl.google.com/android/ndk/android-ndk-r9d-linux-x86_64.tar.bz2
    tar -xjf android-ndk-r9d-linux-x86_64.tar.bz2
    export NDK_HOME=$PWD/android-ndk-r9d
    export PATH=$NDK_HOME:$PATH

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
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    brew install autoconf automake libtool makedepend cmake pkg-config node git
fi

# turn off a few warnings
export CXXFLAGS="-Wno-unknown-warning-option -Wno-unused-local-typedefs -Wno-unknown-pragmas ${CXXFLAGS}";
