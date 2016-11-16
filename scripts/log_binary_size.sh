#!/usr/bin/env bash

set -e
set -o pipefail

function filesize {
    if [ `uname -s` = 'Darwin' ]; then
        stat -f%z $1
    else
        stat --printf=%s $1
    fi
}

# Uploads build metrics to CloudWatch
FILE=$1
DIMENSIONS=$2

if [ -z "${DIMENSIONS}" ]; then
    echo "* No dimensions specified for '${FILE}'"
    exit 1
fi

if [ -f "${FILE}" ]; then
    SIZE=`filesize ${FILE}`
    echo "* Reporting `LC_NUMERIC=en_US printf "%'10.f\n" ${SIZE}` bytes for '${DIMENSIONS}' (${FILE})"
    if [ ${CLOUDWATCH} ]; then
        aws --region us-east-1 cloudwatch put-metric-data \
            --namespace "Mapbox/GL" \
            --metric-name "BinarySize" \
            --unit "Bytes" \
            --value ${SIZE} \
            --dimensions "${DIMENSIONS}"
    fi
else
    echo "* File '${FILE}' does not exist"
fi
