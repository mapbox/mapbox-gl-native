#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    export LD_LIBRARY_PATH=`mason prefix mesa 10.4.3`/lib:$LD_LIBRARY_PATH;
    glxinfo;
fi
