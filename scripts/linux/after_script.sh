#!/usr/bin/env bash

set -e
set -o pipefail

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for uploading the results
    mapbox_time "install_awscli" \
    pip install --user awscli
    export PATH="`python -m site --user-base`/bin:${PATH}"

    mapbox_time_start "deploy_results"
    (cd ./test/suite/ && ./bin/deploy_results.sh)
    mapbox_time_finish
fi
