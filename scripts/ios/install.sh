#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

mapbox_time "install_xcpretty" \
gem install xcpretty --no-rdoc --no-ri --no-document --quiet

mapbox_time "install_recent_git" \
brew install git

mapbox_time "install_awscli" \
brew install awscli

mapbox_time "install_appledoc" \
wget https://github.com/tomaz/appledoc/releases/download/v2.2-963/appledoc.zip && \
tar xvzf appledoc.zip && \
cp appledoc /usr/local/bin && \
cp -Rf Templates/ ~/.appledoc

mkdir -p ${KIF_SCREENSHOTS}
