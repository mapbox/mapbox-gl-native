#!/usr/bin/env bash

set -e
set -o pipefail
set -u

if [ -z "$(which ios-deploy)" ] ; then
    npm install -g ios-deploy
fi

(cd platform/ios/benchmark/assets/tiles && ./download.sh)
(cd platform/ios/benchmark/assets/glyphs && ./download.sh)

make ibench

ios-deploy --debug --noninteractive --bundle "gyp/build/Release-iphoneos/Bench GL.app"
