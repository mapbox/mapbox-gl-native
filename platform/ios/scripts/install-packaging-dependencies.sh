#!/usr/bin/env bash

set -euo pipefail

JAZZY_VERSION="0.10.0"

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

step "Installing packaging dependencies…"

if [ -z `which aws` ]; then
    brew install awscli
fi

if [ -z `which wget` ]; then
    brew install wget
fi

if [[ -z `which jazzy` || $(jazzy -v) != "jazzy version: ${JAZZY_VERSION}" ]]; then
    step "Installing jazzy…"

    CIRCLECI=${CIRCLECI:-false}
    if [[ "${CIRCLECI}" == true ]]; then
        sudo gem install jazzy -v $JAZZY_VERSION --no-document -- --with-sqlite3-lib=/usr/lib
    else
        gem install jazzy -v $JAZZY_VERSION --no-document
    fi

    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy ($JAZZY_VERSION). See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/ios/INSTALL.md"
        exit 1
    fi
fi
