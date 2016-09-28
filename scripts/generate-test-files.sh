#!/usr/bin/env bash

set -e
set -o pipefail

echo "# Do not edit. Regenerate this with ./scripts/generate-test-files.sh" > cmake/test-files.cmake
echo "" >> cmake/test-files.cmake
echo "set(MBGL_TEST_FILES" >> cmake/test-files.cmake
PREFIX=
for FILE in $(git ls-files "test/*.hpp" "test/*.cpp" "test/*.h" "test/*.c" | sort) ; do
    CURRENT_PREFIX=$(dirname ${FILE#test/})
    if [ "${PREFIX}" != "${CURRENT_PREFIX}" ]; then
        if [ ! -z "${PREFIX}" ]; then echo "" >> cmake/test-files.cmake ; fi
        echo "    # ${CURRENT_PREFIX}" >> cmake/test-files.cmake
        PREFIX="${CURRENT_PREFIX}"
    fi
    echo "    ${FILE}" >> cmake/test-files.cmake
done
echo ")" >> cmake/test-files.cmake
git diff cmake/test-files.cmake
