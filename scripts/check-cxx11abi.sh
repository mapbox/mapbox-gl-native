#!/usr/bin/env bash

set -e
set -o pipefail

if [ ! `uname -s` = 'Linux' ]; then
    echo ""
    exit 0
fi

# check-cxx11abi.dat is a binary just so we can use the loader
# to take care of finding the libstdc++ which can be tricky.
LIBSTDCPP=$(ldd $(dirname $0)/check-cxx11abi.dat |grep libstdc++ |cut -d' ' -f3)

if [ $(readelf -Ws $LIBSTDCPP |c++filt |grep -c ::__cxx11::) -gt 0 ]; then
    echo ""
else
    echo "-cxx03abi"
fi
