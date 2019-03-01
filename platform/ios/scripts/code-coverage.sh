#!/usr/bin/env bash

set -e
set -o pipefail

# Get code coverage, then convert it to JSON.
cov_result=build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
xcrun xccov view $cov_result --json > output.json

#
# Convert the line coverage for the dynamic target to a percentage.
# 
percentage=`node -e "console.log(require('./output.json').lineCoverage)"`
cov=$(printf "%.2f" $(echo "$percentage*100" | bc -l))

#
# Create a formatted JSON file with the current coverage. 
#
fileName="ios-coverage-$(TZ=UTC date +"%m-%d-%Y-%H%M")"
cat <<EOF > $fileName.json
{ "current-coverage" : $cov }
EOF
echo $cov
echo $fileName
# Clean up files.
rm -rf build/ios/ios/Logs/Test/*.xcresult/
rm -f output.json

#
# upload to AWS
# 
if [ -z `which aws` ]; then
    brew install awscli
fi
aws s3 cp $fileName.json s3://mapbox-loading-dock/raw/mobile.coverage/ --acl public-read