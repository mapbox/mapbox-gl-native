#!/usr/bin/env bash

set -e
set -o pipefail

node scripts/

aws s3 cp test/fixtures/styles/ s3://mapbox-gl-testing/headless/$TRAVIS_BUILD_NUMBER/$TRAVIS_JOB_NUMBER/ --acl public-read --recursive --exclude "*" --include "*.actual.png" --include "*.html"


echo http://mapbox-gl-testing.s3.amazonaws.com/headless/$TRAVIS_BUILD_NUMBER/$TRAVIS_JOB_NUMBER/index.html
