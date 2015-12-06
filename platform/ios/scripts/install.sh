#!/usr/bin/env bash

set -e
set -o pipefail

git fetch --tags

mapbox_time "checkout_mason" \
git submodule update --init .mason

mapbox_time "install_recent_git" \
brew install git

mapbox_time "install_awscli" \
brew install awscli

mapbox_time "install_jazzy" \
gem install jazzy

mkdir -p ${KIF_SCREENSHOTS}
