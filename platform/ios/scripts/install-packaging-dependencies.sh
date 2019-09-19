#!/usr/bin/env bash

set -euo pipefail

COCOAPODS_VERSION="1.7.5"
JAZZY_VERSION="0.11.1"
CIRCLECI=${CIRCLECI:-false}

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

step "Installing packaging dependencies…"

##
## aws
##
if [ -z `which aws` ]; then
    brew install awscli
else
    echo "Found awscli"
fi

##
## cocoapods
##
if [[ -z `which pod` || $(pod --version) != "${COCOAPODS_VERSION}" ]]; then
    step "Installing cocoapods…"

    if [[ "${CIRCLECI}" == true ]]; then
        sudo gem install cocoapods -v $COCOAPODS_VERSION --no-document
    else
        gem install cocoapods -v $COCOAPODS_VERSION --no-document
    fi

    if [ -z `which pod` ]; then
        echo "Unable to install cocoapods ($COCOAPODS_VERSION)."
        exit 1
    fi
else
    echo "Found cocoapods (${COCOAPODS_VERSION})"
fi

##
## jazzy
##
if [[ -z `which jazzy` || $(jazzy -v) != "jazzy version: ${JAZZY_VERSION}" ]]; then
    step "Installing jazzy…"

    if [[ "${CIRCLECI}" == true ]]; then
        sudo gem install jazzy -v $JAZZY_VERSION --no-document
    else
        gem install jazzy -v $JAZZY_VERSION --no-document
    fi

    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy ($JAZZY_VERSION). See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/ios/INSTALL.md"
        exit 1
    fi
else
    echo "Found jazzy (${JAZZY_VERSION})"
fi

step "Finished installing packaging dependencies"
