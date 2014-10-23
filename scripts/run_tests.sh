#!/usr/bin/env bash

set -e
set -o pipefail

# allow writing core files
ulimit -c unlimited -S

cd build/${BUILDTYPE:-Release}

for TEST in ./test_* ; do
    RESULT=0
    ${TEST} || RESULT=$?

    # output core dump if we got one
    for DUMP in $(find ./ -maxdepth 1 -name 'core*' -print); do
        gdb ${TEST} ${DUMP} -ex "thread apply all bt" -ex "set pagination 0" -batch
        rm -rf ${DUMP}
    done

    # now we should present travis with the original
    # error code so the run cleanly stops
    if [[ ${RESULT} != 0 ]]; then
        exit $RESULT
    fi
done
