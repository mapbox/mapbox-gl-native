#!/usr/bin/env bash

set -e
set -o pipefail

# Get code coverage, then convert it to JSON.
cov_result=build/ios/Logs/Test/*.xcresult/*_Test/*.xccovreport
xcrun xccov view $cov_result --json > output.json

# Access the overall line coverage for the dynamic build.
percentage=`node -e "console.log(require('./output.json').lineCoverage)"`

# Convert the coverage from "0.x" format to a percentage.
cov=$(printf "%.2f" $(echo "$percentage*100" | bc -l))
echo $cov

# Clean up files.
rm -rf build/ios/ios/Logs/Test/*.xcresult/
rm -f output.json