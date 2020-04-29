#!/bin/bash

source="$1"
date=`date +%Y-%m-%d`
file_path="$2"
if [ -z "$file_path" ] || [ -z "$source" ]; then
    >&2 echo "Usage: publish.sh source date filepath"
    >&2 echo ""
    >&2 echo "Example: publish.sh mobile_binarysize android-binarysize.json.gz"
    exit 1
fi
filename="${file_path##*/}"
aws s3 cp "${file_path}" "s3://mapbox-loading-dock/raw/${source}/${date}/${filename}"
