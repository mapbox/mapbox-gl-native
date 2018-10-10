#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=expected
VERSION=e3af253d19db5015a102af88675c063acb9c553e
ROOT=expected-lite-$VERSION

download "https://github.com/martinmoene/expected-lite/archive/$VERSION.tar.gz"
init
extract_gzip "$ROOT/include" "$ROOT/LICENSE.txt"
file_list include -name "*.hpp"
