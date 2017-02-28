#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# Logs metrics on memory usage to CloudWatch

GTEST_OUTPUT=$1
DIMENSIONS=$2

if [ -z "${DIMENSIONS}" ]; then
    echo "* No dimensions specified for memory benchmarks"
    exit 1
fi

function reportAttributeValue {
    ATTR_NAME=$1
    ATTR_UNITS=$2
    ATTR_VALUE=$(cat $GTEST_OUTPUT | grep -o "$ATTR_NAME=\"[^\"]*" | sed "s/$ATTR_NAME=\"//")
    if [ ${CLOUDWATCH:-} ]; then
        echo "* Reporting $ATTR_NAME = $ATTR_VALUE $ATTR_UNITS for '${DIMENSIONS}'"
        aws --region us-east-1 cloudwatch put-metric-data \
            --namespace "Mapbox/GL" \
            --metric-name "$ATTR_NAME" \
            --unit "$ATTR_UNITS" \
            --value ${ATTR_VALUE} \
            --dimensions "${DIMENSIONS}"
    else
        echo "* Measured $ATTR_NAME = $ATTR_VALUE $ATTR_UNITS for '${DIMENSIONS}'"
    fi
}

if [ -f "${GTEST_OUTPUT}" ]; then
    reportAttributeValue vectorFootprint Bytes
    reportAttributeValue rasterFootprint Bytes
else
    echo "* File '${GTEST_OUTPUT}' does not exist"
fi
