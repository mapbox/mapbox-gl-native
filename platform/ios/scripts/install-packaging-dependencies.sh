#!/usr/bin/env bash

set -euo pipefail

JAZZY_VERSION="0.9.5"

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

step "Installing packaging dependencies…"
brew install awscli wget

if [ -z `which jazzy` ]; then
    step "Installing jazzy…"

    CIRCLECI=${CIRCLECI:-false}
    if [[ "${CIRCLECI}" == true ]]; then
        sudo gem install sqlite3 -- --with-sqlite3-lib=/usr/lib
        sudo gem install jazzy -v $JAZZY_VERSION --no-document
    else
        gem install jazzy -v $JAZZY_VERSION --no-document
    fi

    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy ($JAZZY_VERSION). See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/ios/INSTALL.md"
        exit 1
    fi
fi
