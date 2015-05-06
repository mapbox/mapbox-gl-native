#!/usr/bin/env bash

set -e
set -o pipefail

source ./scripts/travis_helper.sh

mapbox_time "checkout_mason" \
git submodule update --init .mason
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

mapbox_time "install_xcpretty" \
gem install xcpretty --no-rdoc --no-ri --no-document --quiet

mapbox_time "install_recent_git" \
brew install git

mapbox_time "install_awscli" \
brew install awscli

mkdir -p ${KIF_SCREENSHOTS}
