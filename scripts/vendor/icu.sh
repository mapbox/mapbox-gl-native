#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=icu
VERSION=61.1
ROOT=icu/source

download "http://download.icu-project.org/files/icu4c/$VERSION/icu4c-${VERSION//./_}-src.tgz"
init
STRIP_COMPONENTS=2 extract_gzip "${ROOT}/common/*.h" "${ROOT}/common/*.cpp"
STRIP_COMPONENTS=1 extract_gzip "icu/LICENSE"

# List all files we want to extract
FILES=(
    ushape.cpp
    ubidi.cpp
    ubidi_props.cpp
    ubidiln.cpp
    ubidiwrt.cpp
    ustring.cpp
    uinvchar.cpp
    uchar.cpp
    udataswp.cpp
    umath.cpp
    utrie2.cpp
    utf_impl.cpp
    utypes.cpp
    cmemory.cpp
    cstring.cpp
)

# Find dependencies for all of these files
echo ">> Finding dependencies..."
ALL=()
for FILE in "${FILES[@]}"; do 
    ALL+=($(cd "common" && $CXX -std=c++14 -I. -c "$FILE" -M | sed -e 's/^[a-z0-9._-]*: *//;s/ *\\$//'))
done

# Remove duplicates
IFS=$'\n' ALL=($(sort <<< "${ALL[*]}" | uniq | sed '/^\//d'))
unset IFS

echo ">> Copying files..."
mkdir -p src include/unicode
for FILE in "${ALL[@]}"; do
    [[ "$FILE" = unicode/* ]] && DIR="include" || DIR="src"
    # Copy file and replace #include "unicode/*" with #include <unicode/*> so that we can separate
    # includes and source-only files.
    sed 's/^#include \"\(unicode\/[^\"]\{1,\}\)\"/#include <\1>/' "common/$FILE" > "$DIR/$FILE"
done

# Apply patch from https://github.com/LibreOffice/core/blob/master/external/icu/icu4c-ubsan.patch.1
# Shifting signed int to a number greater than can be represented is undefined behavior
patch -p0 << PATCH
--- src/ubidiimp.h
+++ src/ubidiimp.h
@@ -198,8 +198,8 @@
 /* in a Run, logicalStart will get this bit set if the run level is odd */
 #define INDEX_ODD_BIT (1UL<<31)

-#define MAKE_INDEX_ODD_PAIR(index, level) ((index)|((int32_t)(level)<<31))
+#define MAKE_INDEX_ODD_PAIR(index, level) ((index)|((uint32_t)(level)<<31))
-#define ADD_ODD_BIT_FROM_LEVEL(x, level)  ((x)|=((int32_t)(level)<<31))
+#define ADD_ODD_BIT_FROM_LEVEL(x, level)  ((x)|=((uint32_t)(level)<<31))
 #define REMOVE_ODD_BIT(x)                 ((x)&=~INDEX_ODD_BIT)

 #define GET_INDEX(x)   ((x)&~INDEX_ODD_BIT)
PATCH

rm -rf common

file_list include src -name "*.h" -o -name "*.cpp"
