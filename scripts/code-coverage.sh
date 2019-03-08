#!/usr/bin/env bash

set -e
set -o pipefail

# Check that input values are valid.

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

# If this is being executed by CI, upload the file to AWS.
if [ ! -z $CIRCLE_SHA]; then
    aws s3 cp $file_name.gz s3://mapbox-loading-dock/raw/mobile.codecoverage/$current_date/
    echo $
fi

