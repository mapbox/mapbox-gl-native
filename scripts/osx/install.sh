#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

mapbox_time "install_xcpretty" \
gem install xcpretty --no-rdoc --no-ri --no-document --quiet
