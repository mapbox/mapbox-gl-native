#!/usr/bin/env bash

set -e
set -o pipefail

# Get code coverage 

# xcodebuild -workspace platform/ios/ios.xcworkspace -scheme CI -enableCodeCoverage YES build test -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' test CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO

# echo build/Logs/Test/*.xcresult
# result=
# echo ${result}*_Test/*.xccovreport
cov_result=build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
echo $cov_result
xcrun xccov view $cov_result --json > output.json

# json
percentage=`node -e "console.log(require('./output.json').lineCoverage)"`

echo "$(($percentage * 100))"

rm -rf build/ios/ios/Logs/Test/*.xcresult/