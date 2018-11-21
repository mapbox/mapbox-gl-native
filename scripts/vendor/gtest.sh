#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=gtest
VERSION=1.8.0
ROOT=googletest-release-1.8.0/googletest

download "https://github.com/google/googletest/archive/release-$VERSION.tar.gz"
init
STRIP_COMPONENTS=2 extract_gzip --exclude "gtest-all.cc" --exclude "gtest_main.cc" "$ROOT/include/*.h" "$ROOT/src" "$ROOT/LICENSE"

for FILE in $(find include src -name "*.h" -o -name "*.cc"); do
    sed -e 's/^#include \"src\/\([^\"]\{1,\}\)\"/#include "\1"/' \
        -e 's/^#include \"\(gtest\/[^\"]\{1,\}\)\"/#include <\1>/' \
        "$FILE" > "$FILE.bak" && mv "$FILE.bak" "$FILE"
done

file_list include src -name "*.h" -o -name "*.cc"

