#!/usr/bin/env bash

set -e
set -o pipefail
set -u

export PUBLISH_VERSION=$1
export BINARY_DIRECTORY=$2
export ZIP_OUTPUT=mapbox-ios-sdk-${PUBLISH_VERSION}-fabric
export FILE_NAME=mapbox-ios-sdk-${PUBLISH_VERSION}-fabric.zip
export ZIP_ARCHIVE_PATH=${BINARY_DIRECTORY}/${FILE_NAME}
export BUNDLE_ID="com.mapbox.sdk.ios"

echo "Downloading ${FILE_NAME}:"
wget -P ${BINARY_DIRECTORY} http://mapbox.s3.amazonaws.com/mapbox-gl-native/ios/builds/${FILE_NAME}

echo "Extracting ${ZIP_ARCHIVE_PATH} to ${BINARY_DIRECTORY}/${ZIP_OUTPUT}"
unzip -q ${ZIP_ARCHIVE_PATH} -d ${BINARY_DIRECTORY}/${ZIP_OUTPUT}  
ditto ${BINARY_DIRECTORY}/${ZIP_OUTPUT}/static/Mapbox.framework ${BINARY_DIRECTORY}/Mapbox.framework

echo "Zipping framework:"
cd ${BINARY_DIRECTORY}
zip -q -r Mapbox.framework.zip Mapbox.framework
cd $OLDPWD

echo "Validating framework:"
./validate-fabric-zip.sh ${BINARY_DIRECTORY}/Mapbox.framework.zip
 
echo "Uploading ${BINARY_DIRECTORY}/Mapbox.framework.zip to https://kits.fabric.io/manage-api/v1/kit-releases/ios/$BUNDLE_ID/$PUBLISH_VERSION with key ${FABRIC_KIT_API_KEY}"
curl --fail -v -X PUT -H "X-FabricKits-ApiKey: ${FABRIC_KIT_API_KEY}" \
    -F "release_artifact=@${BINARY_DIRECTORY}/Mapbox.framework.zip;type=application/octet-stream" \
    https://kits.fabric.io/manage-api/v1/kit-releases/ios/$BUNDLE_ID/$PUBLISH_VERSION

echo "Cleaning up"
rm -r #{BINARY_DIRECTORY}

echo "Done"
