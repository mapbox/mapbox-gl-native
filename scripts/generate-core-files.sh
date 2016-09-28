#!/usr/bin/env bash

set -e
set -o pipefail

echo "# Do not edit. Regenerate this with ./scripts/generate-core-files.sh" > cmake/core-files.cmake
echo "" >> cmake/core-files.cmake
echo "set(MBGL_CORE_FILES" >> cmake/core-files.cmake
PREFIX=
for FILE in $(git ls-files "include/*.hpp" "include/*.h" "src/*.hpp" "src/*.cpp" "src/*.h" "src/*.c" | perl -p -e "s/^((src|include)\/(mbgl\/)?(.+)\/\w+\.\w+)$/\$4#\$1/g" | sort) ; do
    CURRENT_PREFIX="${FILE%#*}"
    if [ "${PREFIX}" != "${CURRENT_PREFIX}" ]; then
        if [ ! -z "${PREFIX}" ]; then echo "" >> cmake/core-files.cmake ; fi
        echo "    # ${CURRENT_PREFIX}" >> cmake/core-files.cmake
        PREFIX="${CURRENT_PREFIX}"
    fi
    echo "    ${FILE#*#}" >> cmake/core-files.cmake
done
echo ")" >> cmake/core-files.cmake
git diff cmake/core-files.cmake
