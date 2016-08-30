#!/usr/bin/env bash

set -e
set -o pipefail

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -y
sudo apt-get install -y gdb g++-4.9 gcc-4.9 libllvm3.4 xutils-dev libxxf86vm-dev x11proto-xf86vidmode-dev mesa-utils
