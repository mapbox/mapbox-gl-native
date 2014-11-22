#!/usr/bin/env bash

set -e
set -o pipefail

echo "submitting testrun to testmunk"

echo "uploading features"
cd android/test
zip -r features.zip features/
curl -H 'Accept: application/vnd.testmunk.v1+json' -F 'file=@features.zip' 'https://If1ONAxGqcIgiI1vtK468V54ERJf1xem@api.testmunk.com/apps/Testmunk/testcases'
cd ../..

echo "uploading apk"
cd android/java/app/build/outputs/apk/
curl -H 'Accept: application/vnd.testmunk.v1+json' -F 'file=@app-debug.apk' -F 'email=leith@mapbox.com' -F 'autoStart=true' -F 'public=true' 'https://If1ONAxGqcIgiI1vtK468V54ERJf1xem@api.testmunk.com/apps/Mapbox/testruns'
cd ../../../../../..
