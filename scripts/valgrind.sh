#!/usr/bin/env bash

set -e
set -o pipefail

.mason/mason install valgrind 3.12.0

PARAMS="\
    --leak-check=full \
    --show-leak-kinds=definite \
    --errors-for-leak-kinds=definite \
    --error-exitcode=1 \
    --gen-suppressions=all \
    --suppressions=scripts/valgrind.sup"

export VALGRIND_LIB=$(.mason/mason prefix valgrind 3.12.0)/lib/valgrind

$(.mason/mason prefix valgrind 3.12.0)/bin/valgrind $PARAMS $@
