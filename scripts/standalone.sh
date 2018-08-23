#!/usr/bin/env bash

set -e
set -o pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."

function usage {
    echo "Usage: $0 <args> [target]"
    echo ""
    echo "    -c, --clean-target           Removes all files in the target directory instead of overwriting them"
    echo "    -s, --skip [name]            Skips vendoring the given dependency"
    exit 1
}

CLEAN_TARGET=false

ARGS=()
while [[ $# -gt 0 ]] ; do case "$1" in
    -c|--clean-target) CLEAN_TARGET=true ; shift ;;
    -s|--skip) shift ; DEP="$1" ; shift ; declare SKIP_$DEP=true ;;
    *) ARGS+=("$1") ; shift ;;
esac ; done
set -- "${ARGS[@]}"

if [ -z "$1" ]; then usage ; fi
TARGET="$1"

if [ "$CLEAN_TARGET" = true ] ; then
    echo ">> Cleaning target directory..."
    rm -rf "$TARGET"
fi

echo ">> Creating target directory..."
mkdir -p "$TARGET"
TARGET=$(cd "`pwd`/$TARGET"; pwd)

echo ">> Creating file lists..."
cat cmake/core-files.txt cmake/filesource-files.txt > "$TARGET/files.txt"
mkdir -p "$TARGET/platform/android"
cat platform/android/*-files.txt > "$TARGET/platform/android/files.txt"

echo ">> Copying source files..."
rsync -rR $(git ls-files \
    "include" \
    "src" \
    "platform/default" \
    "platform/android/mbgl" \
    "platform/android/src" \
    "platform/linux/src" \
    "platform/android/MapboxGLAndroidSDK/src/main" \
) "$TARGET"

echo ">> Copying vendored files..."
rsync -rR $(git ls-files \
    "vendor/expected" \
    "vendor/icu" \
    "vendor/nunicode" \
    "vendor/sqlite" \
) "$TARGET"

echo ">> Copying Mason dependencies..."
cmake -P scripts/standalone.cmake 2>&1 | while read DEPENDENCY ; do
    DEPENDENCY=($DEPENDENCY)
    if [ "[Mason]" = "${DEPENDENCY[0]}" ]; then
        echo "${DEPENDENCY[@]}"
        continue
    fi
    SKIP=SKIP_${DEPENDENCY[0]}
    if [ "${!SKIP:-false}" = true ]; then continue ; fi
    DESTINATION="$TARGET/vendor/${DEPENDENCY[0]}"
    mkdir -p "$DESTINATION"
    echo "   - ${DEPENDENCY[0]}"
    cp -r "${DEPENDENCY[1]}/" "$DESTINATION"
    rm "$DESTINATION/mason.ini"
    (cd "${DEPENDENCY[1]}" && find include -type f -name "*.hpp" -o -name "*.h") > "$DESTINATION/files.txt"
    echo "${DEPENDENCY[2]}" > "$DESTINATION/version.txt"
done
