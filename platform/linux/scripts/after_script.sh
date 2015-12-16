#!/bin/bash

set -e
set -o pipefail

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for uploading the results
    pip install --user awscli
    export PATH="`python -m site --user-base`/bin:${PATH}"

    REPO_NAME=$(basename $TRAVIS_REPO_SLUG)

    aws s3 cp --recursive --acl public-read test/fixtures/annotations \
        s3://mapbox/$REPO_NAME/render-tests/$TRAVIS_JOB_NUMBER/annotations
fi
