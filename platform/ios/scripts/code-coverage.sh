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
file_name="ios-coverage-$(TZ=UTC date +"%Y-%m-%d-$CIRCLE_SHA1")"
cat <<EOF > $file_name.json
    { "current-coverage" : $cov }
EOF

gzip -f $file_name.json
#
# upload to AWS
# 

aws s3 cp $file_name.json.gz s3://mapbox-loading-dock/raw/mobile.coverage/