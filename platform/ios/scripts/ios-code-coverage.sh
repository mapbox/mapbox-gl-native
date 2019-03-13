#!/usr/bin/env bash

set -e
set -o pipefail

#
# Get code coverage report, then convert it to JSON.
# The coverage reports end up in different locations based on whether coverage
# is generated via CI or locally.
#
cov_result="";
if [ -f build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport ]; then 
    cov_result=build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
elif [ -f build/ios/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport ]; then
    cov_result=build/ios/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
else
    echo "Coverage file does not exist. Please run tests before executing"
    exit 1
fi

xcrun xccov view $cov_result --json > output.json

#
# Convert the line coverage for the dynamic target to a percentage. Currently, 
# only CI tests are included when calculated code coverage.
# 
percentage=`node -e "console.log(require('./output.json').lineCoverage)"`
cov=$(printf "%.2f" $(echo "$percentage*100" | bc -l))

# Generate a formatted JSON file and upload it to S3.
./././scripts/code-coverage.sh $cov "iOS" "$1"
