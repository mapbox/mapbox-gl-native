#!/usr/bin/env bash

if [[ `lsb_release -r` =~ "12.04" ]]; then
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
fi
sudo apt-get update -y
sudo apt-get -y install gcc-4.8 g++-4.8
sudo apt-get -y install pkg-config nodejs cmake automake libtool xutils-dev git
sudo apt-get -y install libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev
