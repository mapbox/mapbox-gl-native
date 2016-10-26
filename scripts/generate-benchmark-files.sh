#!/usr/bin/env bash

set -e
set -o pipefail

echo "# Do not edit. Regenerate this with ./scripts/generate-benchmark-files.sh" > cmake/benchmark-files.cmake
echo "" >> cmake/benchmark-files.cmake
echo "set(MBGL_BENCHMARK_FILES" >> cmake/benchmark-files.cmake
PREFIX=
for FILE in $(git ls-files "benchmark/*.hpp" "benchmark/*.cpp" "benchmark/*.h" "benchmark/*.c" | sort) ; do
    CURRENT_PREFIX=$(dirname ${FILE#benchmark/})
    if [ "${PREFIX}" != "${CURRENT_PREFIX}" ]; then
        if [ ! -z "${PREFIX}" ]; then echo "" >> cmake/benchmark-files.cmake ; fi
        echo "    # ${CURRENT_PREFIX}" >> cmake/benchmark-files.cmake
        PREFIX="${CURRENT_PREFIX}"
    fi
    echo "    ${FILE}" >> cmake/benchmark-files.cmake
done
echo ")" >> cmake/benchmark-files.cmake
git diff cmake/benchmark-files.cmake
