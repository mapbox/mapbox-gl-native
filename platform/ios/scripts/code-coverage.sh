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

# not sure how to round
cov="${percentage:2:2}.${percentage:3:4}"
echo $cov
rm -rf build/ios/ios/Logs/Test/*.xcresult/
rm -f output.json