#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=expected
VERSION=a2359cf61478d735a308e952b1c9840714f61386
ROOT=expected-lite-$VERSION

download "https://github.com/martinmoene/expected-lite/archive/$VERSION.tar.gz"
init
extract_gzip "$ROOT/include" "$ROOT/LICENSE.txt"
mv include/nonstd/expected.hpp include
rm -rf include/nonstd
file_list include -name "*.hpp"
