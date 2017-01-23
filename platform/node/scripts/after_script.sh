#!/bin/bash

set -e
set -o pipefail

JOB=$1

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    gzip --stdout mapbox-gl-js/test/integration/render-tests/index.html | \
        aws s3 cp --acl public-read --content-encoding gzip --content-type text/html \
            - s3://mapbox/mapbox-gl-native/render-tests/$JOB/index.html

    echo http://mapbox.s3.amazonaws.com/mapbox-gl-native/render-tests/$JOB/index.html
fi
