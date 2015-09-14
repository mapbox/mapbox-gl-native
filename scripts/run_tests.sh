#!/usr/bin/env bash

set -e
set -o pipefail

if [ `uname -s` = 'Darwin' ]; then HOST=${HOST:-osx} ; else HOST=${HOST:-linux} ; fi

CMD=$@
shift

# allow writing core files
ulimit -c unlimited -S
echo 'ulimit -c: '`ulimit -c`
if [ -f /proc/sys/kernel/core_pattern ]; then
    echo '/proc/sys/kernel/core_pattern: '`cat /proc/sys/kernel/core_pattern`
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    sysctl kernel.core_pattern
fi

# install test server dependencies
if [ ! -d "test/node_modules/express" ]; then
    (cd test; npm install express@4.11.1)
fi

if which -s 'gdb'; then
    gdb -batch -return-child-result -ex 'set print thread-events off' \
        -ex 'run' -ex 'thread apply all bt' --args ${CMD} ;
else
    ${CMD} ;
fi
