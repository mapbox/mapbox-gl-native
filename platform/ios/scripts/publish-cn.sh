
#!/usr/bin/env bash

set -e
set -o pipefail
set -u

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

#
# iOS 8-compatible release tag format is `vX.Y.Z-cn.1`; `X.Y.Z-cn.1` gets passed in
# In the case of symbolicated builds, we also append the `-symbols`.
#
PUBLISH_VERSION="$1"

if [[ ${#} -eq 2 ]]; then
    PUBLISH_STYLE="-$2"
else
    PUBLISH_STYLE=""
fi

#
# make the framework
#
  
step "Make the framework"
export BUILDTYPE=Release
./platform/ios/scripts/package.sh
echo "making package"
wait
echo "finished!"

#
# zip
#
cd build/ios/pkg
ZIP="mapbox-ios-sdk-${PUBLISH_VERSION}.zip"
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

aws s3 cp ../${ZIP} s3://binary.mapbox.cn/ios/builds/maps-sdk/ --acl public-read ${PROGRESS}
echo "URL:  https://s3.cn-north-1.amazonaws.com.cn/binary.mapbox.cn/ios/builds/maps-sdk/${ZIP}"
