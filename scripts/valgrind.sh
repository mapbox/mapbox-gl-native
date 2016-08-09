#!/usr/bin/env bash

set -e
set -o pipefail

.mason/mason install valgrind latest

PARAMS="\
    --leak-check=full \
    --show-leak-kinds=definite \
    --errors-for-leak-kinds=definite \
    --error-exitcode=1 \
    --gen-suppressions=all \
    --suppressions=scripts/valgrind.sup"

export VALGRIND_LIB=$(.mason/mason prefix valgrind latest)/lib/valgrind

$(.mason/mason prefix valgrind latest)/bin/valgrind $PARAMS $@
