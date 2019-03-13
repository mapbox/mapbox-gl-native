#!/usr/bin/env bash

set -e
set -o pipefail

#
# This script takes three values: $1 should be a decimal value reflecting the 
# percentage of lines covered, with a maximum value of 100.0, $2 is
# the platform the percentage pertains to (iOS or Android), and $3 is the
# test scheme being run (on iOS, this is currently CI).
#

# Check that platform input values are valid.

if [[ ! $2 = "iOS" && ! $2 = "Android" ]]; then
    echo "$2 does not match either 'iOS' or 'Android'. Platform must be specified for coverage report."
    exit 1
fi

# Create a formatted JSON file that contains the current coverage. 

current_date=$(TZ=UTC date +"%Y-%m-%d")
file_name=$2_coverage.json
cat <<EOF > $file_name
    {"code_coverage":$1,"platform":"$2","sdk":"Maps","scheme":"$3","created_at":"$current_date"}
EOF
gzip -f $file_name

if [ -z `which aws` ]; then	
    brew install awscli	
fi

aws s3 cp $file_name.gz s3://mapbox-loading-dock/raw/mobile.codecoverage/$current_date/
echo $

