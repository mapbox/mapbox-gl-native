#!/usr/bin/env bash

set -e
set -o pipefail
set -u

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

#
# Update the PUBLISH_VERSION manually when releasing a new iOS 8 compatible SDK.
# In the case of symbolicated builds, we also append the `-symbols`.
#
# zip
#
cd build/ios/pkg
ZIP="mapbox-ios-sdk-cn.zip"
step "Compressing ${ZIP}…"
rm -f ../${ZIP}
zip -yr ../${ZIP} *

#
# report file sizes
#
step "Echoing file sizes…"
du -sh ../${ZIP}
du -sch *
du -sch dynamic/*

#
# upload
#
PROGRESS=
if [ -n "${CI:-}" ]; then
    PROGRESS="--no-progress"
fi

step "Uploading ${ZIP} to s3…"
# Since this build is primarily for .CN customers, it will be hosted on .cn. 

aws s3 cp ../${ZIP} s3://binary.mapbox.cn/mapbox-china-plugin/ios/builds/maps-sdk/ --acl public-read ${PROGRESS}
echo "URL: https://mapbox.s3.amazonaws.cn/mapbox-plugin-ios/ios/builds/maps-sdk/${ZIP}"
