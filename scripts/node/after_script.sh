#!/bin/bash

set -e
set -o pipefail

# Inspect binary.
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    ldd ./lib/mapbox-gl-native.node
else
    otool -L ./lib/mapbox-gl-native.node
fi

COMMIT_MESSAGE=$(git show -s --format=%B $TRAVIS_COMMIT | tr -d '\n')
PACKAGE_JSON_VERSION=$(node ./scripts/node/package-version.js)

if [[ ${TRAVIS_TAG} == node-v${PACKAGE_JSON_VERSION} ]] || test "${COMMIT_MESSAGE#*'[publish binary]'}" != "$COMMIT_MESSAGE"; then
    source ~/.nvm/nvm.sh
    nvm use $NODE_VERSION

    npm install aws-sdk

    ./node_modules/.bin/node-pre-gyp package

    if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
        ./node_modules/.bin/node-pre-gyp testpackage
    fi

    ./node_modules/.bin/node-pre-gyp publish info

    if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
        source ./scripts/${TRAVIS_OS_NAME}/setup.sh

        rm -rf build
        rm -rf lib
        npm install --fallback-to-build=false
        npm test
    fi
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]] && [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for uploading the results
    pip install --user awscli
    export PATH="`python -m site --user-base`/bin:${PATH}"

    REPO_NAME=$(basename $TRAVIS_REPO_SLUG)
    gzip --stdout node_modules/mapbox-gl-test-suite/render-tests/index.html | \
        aws s3 cp --acl public-read --content-encoding gzip --content-type text/html \
            - s3://mapbox/$REPO_NAME/render-tests/$TRAVIS_JOB_NUMBER/index.html

    echo http://mapbox.s3.amazonaws.com/$REPO_NAME/render-tests/$TRAVIS_JOB_NUMBER/index.html
fi
