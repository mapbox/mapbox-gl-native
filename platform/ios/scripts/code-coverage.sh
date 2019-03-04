#!/usr/bin/env bash

set -e
set -o pipefail

# Get code coverage, then convert it to JSON.
cov_result="";
if [ -f build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport ]; then 
    cov_result=build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
elif [ -f build/ios/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport ]; then
    cov_result=build/ios/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
else
    echo "Coverage file does not exist"
    exit 1
fi

xcrun xccov view $cov_result --json > output.json

#
# Convert the line coverage for the dynamic target to a percentage.
# 
percentage=`node -e "console.log(require('./output.json').lineCoverage)"`
cov=$(printf "%.2f" $(echo "$percentage*100" | bc -l))

#
# Create a formatted JSON file with the current coverage. 
#


cat <<EOF > ios-coverage.json
    { "current-coverage" : $cov }
EOF

gzip -f ios-coverage.json
#
# upload to AWS
# 
current_date=$(TZ=UTC date +"%Y-%m-%d")
aws s3 cp ios-coverage.json.gz s3://mapbox-loading-dock/raw/mobile.coverage/$current_date/