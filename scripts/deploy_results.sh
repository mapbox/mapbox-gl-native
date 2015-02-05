#!/usr/bin/env bash

set -e
set -o pipefail

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    (cd ./node_modules/mapbox-gl-test-suite/ && ./bin/deploy_results.sh)
fi
