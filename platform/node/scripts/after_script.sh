#!/bin/bash

set -e
set -o pipefail

JOB=$1
TAG=$2

if [[ ${PUBLISH} ]]; then
    if [[ "$BUILDTYPE" == "Debug" ]]; then
        echo "Please run this script in release mode (BUILDTYPE=Release)."
        exit 1
    else
        ./node_modules/.bin/node-pre-gyp package publish info
    fi
else
    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
        gzip --stdout node_modules/mapbox-gl-test-suite/render-tests/index.html | \
            aws s3 cp --acl public-read --content-encoding gzip --content-type text/html \
                - s3://mapbox/mapbox-gl-native/render-tests/$JOB/index.html

        echo http://mapbox.s3.amazonaws.com/mapbox-gl-native/render-tests/$JOB/index.html
    fi
fi
