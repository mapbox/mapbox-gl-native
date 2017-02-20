#!/usr/bin/env bash

set -e
set -o pipefail

VALGRIND_PREFIX=$(scripts/mason.sh PREFIX valgrind VERSION 3.12.0)

PARAMS="\
    --leak-check=full \
    --show-leak-kinds=definite \
    --errors-for-leak-kinds=definite \
    --error-exitcode=1 \
    --gen-suppressions=all \
    --suppressions=scripts/valgrind.sup"

export VALGRIND_LIB=${VALGRIND_PREFIX}/lib/valgrind

${VALGRIND_PREFIX}/bin/valgrind $PARAMS $@
