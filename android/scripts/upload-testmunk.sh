#!/usr/bin/env bash

set -e
set -o pipefail

EMAIL="leith@mapbox.com"
APP_NAME="Mapbox"
APK_PATH=$1

echo "submitting testrun to testmunk"

echo "uploading features..."

zip -r android/test/features.zip android/test/features/
RESPONSE=$(curl -f -H "Accept: application/vnd.testmunk.v1+json" -F "file=@android/test/features.zip" "https://${TESTMUNK_KEY}@api.testmunk.com/apps/${APP_NAME}/testcases")

echo "uploading apk..."

RESPONSE=$(curl -f -H "Accept: application/vnd.testmunk.v1+json" -F "file=@${APK_PATH}" -F "email=${EMAIL}" -F "autoStart=true" -F "public=true" "https://${TESTMUNK_KEY}@api.testmunk.com/apps/${APP_NAME}/testruns")

TESTRUN_ID=$(echo "${RESPONSE}" | jq -r '.id')
TESTRUN_NAME=$(echo "${RESPONSE}" | jq -r '.name')

echo "successully uploaded to Testmunk as '${TESTRUN_NAME}'"
echo "public link to test results: https://www.testmunk.com/testrun/${TESTRUN_ID}"
cd -
