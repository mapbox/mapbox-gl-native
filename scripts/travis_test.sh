#!/usr/bin/env bash

set -e
set -o pipefail

# Travis OS X has no GPU
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    npm test
    # ./scripts/deploy_results.sh
fi
