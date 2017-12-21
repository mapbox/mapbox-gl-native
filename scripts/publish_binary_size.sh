#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# Downloads log data from AWS CloudWatch and uploads it as a JSON file to S3 for public access.

function publish_binary_size {
    local DIMENSIONS=$1

    if [ -z "${DIMENSIONS}" ]; then
        echo "* No dimensions specified"
        exit 1
    fi

    function print_dimensions {
        for ITEM in ${DIMENSIONS//,/ } ; do
            echo -n "Name=${ITEM//=/,Value=} "
        done
    }

    local DATE_FORMAT="%Y-%m-%dT%H:%M:%SZ"
    local DATE_END=$(date -u +${DATE_FORMAT})

    if [ `uname -s` = 'Darwin' ]; then # BSD date
        local DATE_BEGIN=$(date -jf "${DATE_FORMAT}" -v-60d "${DATE_END}" +"${DATE_FORMAT}")
    else # GNU date
        local DATE_BEGIN=$(date --date="${DATE_END} - 60 days" +"${DATE_FORMAT}")
    fi

    # Download the metrics, gzip, and upload to S3.
    aws --region us-east-1 cloudwatch get-metric-statistics \
        --namespace "Mapbox/GL" \
        --metric-name "BinarySize" \
        --unit "Bytes" \
        --start-time "${DATE_BEGIN}" \
        --end-time "${DATE_END}" \
        --period 3600 \
        --statistics Maximum \
        --dimensions `print_dimensions` \
    | gzip | aws s3 cp \
        --acl public-read \
        --cache-control "max-age=300" \
        --content-encoding gzip \
        --content-type application/json \
        - "s3://mapbox/mapbox-gl-native/metrics/binary-size/${DIMENSIONS}.json"

    echo "* Uploaded data to 's3://mapbox/mapbox-gl-native/metrics/binary-size/${DIMENSIONS}.json'"
}

if [ $# -gt 0 ]; then
    # Upload the specified dimension only
    publish_binary_size "$1"
else
    # Upload all dimensions that we are tracking
    publish_binary_size "Platform=iOS,Arch=armv7"
    publish_binary_size "Platform=iOS,Arch=arm64"
    publish_binary_size "Platform=iOS,Arch=x86_64"
    publish_binary_size "Platform=iOS,Arch=Dynamic"

    publish_binary_size "Platform=macOS,Arch=x86_64"

    publish_binary_size "Platform=Linux,Compiler=clang-3.8,Arch=x86_64"
    publish_binary_size "Platform=Linux,Compiler=gcc-5,Arch=x86_64"

    publish_binary_size "Platform=Android,Arch=arm-v5"
    publish_binary_size "Platform=Android,Arch=arm-v7"
    publish_binary_size "Platform=Android,Arch=arm-v8"
    publish_binary_size "Platform=Android,Arch=x86"
    publish_binary_size "Platform=Android,Arch=x86_64"
    publish_binary_size "Platform=Android,Arch=mips"
    publish_binary_size "Platform=Android,Arch=Archive"
fi
