#!/usr/bin/env bash

set -e
set -o pipefail
set -u

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
cat <<EOF > coverage.JSON
{ "current-coverage" : $cov }
EOF
echo $cov

# Clean up files.
rm -rf build/ios/ios/Logs/Test/*.xcresult/
rm -f output.json

#
# upload to AWS
# 

aws s3 cp coverage.json s3://mapbox-loading-dock/raw/ios-maps-metrics/ --acl public-read