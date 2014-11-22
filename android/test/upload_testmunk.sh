#!/usr/bin/env bash

set -e
set -o pipefail

APP_NAME = "Mapbox"
APK_PATH = $1

echo "submitting testrun to testmunk"

echo "uploading features"
echo

zip -r features.zip features/
curl -H 'Accept: application/vnd.testmunk.v1+json' -F 'file=@features.zip' 'https://${TESTMUNK_KEY}@api.testmunk.com/apps/${APP_NAME}/testcases'

echo "uploading apk"
echo

cd `dirname ${APK_PATH}`
curl -H 'Accept: application/vnd.testmunk.v1+json' -F 'file=@`basename ${APK_PATH}`' -F 'email=leith@mapbox.com' -F 'autoStart=true' -F 'public=true' 'https://${TESTMUNK_KEY}@api.testmunk.com/apps/${APP_NAME}/testruns'
cd -
