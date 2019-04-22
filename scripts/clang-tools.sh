#!/usr/bin/env bash

set -euo pipefail

CLANG_TIDY_PREFIX=${CLANG_TIDY_PREFIX:-$(scripts/mason.sh PREFIX clang-tidy VERSION 7.0.0)}
CLANG_TIDY=${CLANG_TIDY_PREFIX}/bin/clang-tidy
CLANG_APPLY=${CLANG_TIDY_PREFIX}/bin/clang-apply-replacements

for CLANG_FILE in "${CLANG_TIDY}" "${CLANG_APPLY}"; do
    command -v "${CLANG_FILE}" > /dev/null 2>&1 || {
        echo "Can't find ${CLANG_FILE} in PATH."
        if [ -z "${CLANG_FILE}" ]; then
            echo "Alternatively, you can manually set ${!CLANG_FILE@}."
        fi
        exit 1
    }
done

echo "Running Clang checks... (this might take a while)"

if [[ -n ${3:-} ]] && [[ ${3:-} == "--diff" ]]; then
    git diff "origin/$2" | "scripts/clang-tidy-diff.py" \
        -clang-tidy-binary "${CLANG_TIDY}" \
        -p 1 -quiet -path "$1"
else
    git ls-files "src/*.cpp" "test/*.cpp" "platform/*.cpp" "bin/*.cpp" | \
        xargs "${CLANG_TIDY_PREFIX}/share/run-clang-tidy.py" \
        -clang-tidy-binary "${CLANG_TIDY}" \
        -clang-apply-replacements-binary "${CLANG_APPLY}" \
        -quiet -p "$1" -fix
fi
