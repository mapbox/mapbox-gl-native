#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=nunicode
VERSION=1.7.1
ROOT=alekseyt-nunicode-263219737286

download "https://bitbucket.org/alekseyt/nunicode/get/$VERSION.tar.gz"
init
extract "$ROOT/libnu/*.c" "$ROOT/libnu/*.h" "$ROOT/LICENSE"
mkdir -p include/libnu src/libnu/gen
find libnu -name "*.c" -exec mv {} src/{} \;
find libnu -name "*.h" -exec mv {} include/{} \;
rm -rf libnu
file_list include src -name "*.h" -o -name "*.c"
