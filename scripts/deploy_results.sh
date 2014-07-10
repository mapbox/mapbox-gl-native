#!/usr/bin/env bash

set -e
set -o pipefail

aws s3 cp test/fixtures/styles/ s3://mapbox-gl-testing/headless/$TRAVIS_JOB_NUMBER/ --acl public-read --recursive

echo http://mapbox-gl-testing.s3.amazonaws.com/headless/$TRAVIS_JOB_NUMBER/index.html
