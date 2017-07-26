#!/usr/bin/env bash

set -e
set -o pipefail

CLANG_TIDY_PREFIX=${CLANG_TIDY_PREFIX:-$(scripts/mason.sh PREFIX clang-tidy VERSION 4.0.1)}
CLANG_TIDY=${CLANG_TIDY:-${CLANG_TIDY_PREFIX}/bin/clang-tidy}
CLANG_APPLY=${CLANG_APPLY:-${CLANG_TIDY_PREFIX}/bin/clang-apply-replacements}

CLANG_FORMAT=${CLANG_FORMAT:-$(scripts/mason.sh PREFIX clang-format VERSION 4.0.1)/bin/clang-format}

for CLANG_FILE in "${CLANG_TIDY} ${CLANG_APPLY} ${CLANG_FORMAT}"; do
    command -v ${CLANG_TIDY} > /dev/null 2>&1 || {
        echo "Can't find ${CLANG_FILE} in PATH."
        if [ -z ${CLANG_FILE} ]; then
            echo "Alternatively, you can manually set ${!CLANG_FILE@}."
        fi
        exit 1
    }
done

cd $1

export CDUP=$(git rev-parse --show-cdup)
export CLANG_TIDY CLANG_APPLY CLANG_FORMAT

function run_clang_tidy() {
    FILES=$(git ls-files "src/mbgl/*.cpp" "platform/*.cpp" "test/*.cpp")
    ${CLANG_TIDY_PREFIX}/share/run-clang-tidy.py -j ${JOBS} \
        -clang-tidy-binary ${CLANG_TIDY} \
        -clang-apply-replacements-binary ${CLANG_APPLY} \
        -fix ${FILES} 2>/dev/null || exit 1
}

function run_clang_tidy_diff() {
    OUTPUT=$(git diff origin/master --src-prefix=${CDUP} --dst-prefix=${CDUP} | \
                ${CLANG_TIDY_PREFIX}/share/clang-tidy-diff.py \
                    -clang-tidy-binary ${CLANG_TIDY} \
                    2>/dev/null)
    if [[ -n $OUTPUT ]] && [[ $OUTPUT != "No relevant changes found." ]]; then
        echo -e "${OUTPUT}"
        exit 1
    fi
}

function run_clang_format() {
    echo "Running clang-format on $0..."
    DIFF_FILES=$(git diff origin/master --name-only *cpp)
    echo "${DIFF_FILES}" | xargs -I{} -P ${JOBS} bash -c 'run_clang_format' {}
    ${CLANG_FORMAT} -i ${CDUP}/$0 || exit 1
}

export -f run_clang_tidy run_clang_tidy_diff run_clang_format

echo "Running Clang checks... (this might take a while)"

if [[ -n $2 ]] && [[ $2 == "--diff" ]]; then
    run_clang_tidy_diff $@
    # XXX disabled until we run clang-format over the entire codebase.
    #run_clang_format $@
    echo "All checks pass!"
else
    run_clang_tidy $@
fi
