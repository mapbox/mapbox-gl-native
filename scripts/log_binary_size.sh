#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# Logs metrics on binary size to CloudWatch

FILE=$1
DIMENSIONS=$2

if [ -z "${DIMENSIONS}" ]; then
    echo "* No dimensions specified for '${FILE}'"
    exit 1
fi

function filesize {
    if [ `uname -s` = 'Darwin' ]; then
        stat -f%z $1
    else
        stat --printf=%s $1
    fi
}

if [ -f "${FILE}" ]; then
    SIZE=`filesize ${FILE}`
    if [ ${CLOUDWATCH:-} ]; then
        echo "* Reporting `LC_NUMERIC=en_US printf "%'10.f\n" ${SIZE}` bytes for '${DIMENSIONS}' (${FILE})"
        aws --region us-east-1 cloudwatch put-metric-data \
            --namespace "Mapbox/GL" \
            --metric-name "BinarySize" \
            --unit "Bytes" \
            --value ${SIZE} \
            --dimensions "${DIMENSIONS}"

        ./scripts/publish_binary_size.sh "${DIMENSIONS}"
    else
        echo "* Measured `LC_NUMERIC=en_US printf "%'10.f\n" ${SIZE}` bytes for '${DIMENSIONS}' (${FILE})"
    fi
else
    echo "* File '${FILE}' does not exist"
fi
