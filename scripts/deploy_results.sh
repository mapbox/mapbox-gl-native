#!/usr/bin/env bash

set -e
set -o pipefail

aws s3 cp test/fixtures/styles/ s3://mapbox-gl-testing/headless/$TRAVIS_BUILD_NUMBER/$TRAVIS_JOB_NUMBER/ --acl public-read --recursive --exclude "*" --include "*.png"

cd test/fixtures/styles
for FILE in *.png ; do
    echo http://mapbox-gl-testing.s3.amazonaws.com/headless/$TRAVIS_BUILD_NUMBER/$TRAVIS_JOB_NUMBER/$FILE
done
cd ../../..
