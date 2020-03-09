#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Give base directory as argument, eg. 'build'"
    exit 1
fi

BASE_DIR=$1

rm -rf $BASE_DIR/test-data/

mkdir -p $BASE_DIR/test-data/integration/
mkdir -p $BASE_DIR/test-data/baselines/

cp -r mapbox-gl-js/test/integration/render-tests $BASE_DIR/test-data/integration/
cp -r mapbox-gl-js/test/integration/query-tests $BASE_DIR/test-data/integration/
cp -r metrics/expectations $BASE_DIR/test-data/
cp -r metrics/ignores $BASE_DIR/test-data/
cp -r metrics/tests $BASE_DIR/test-data/
cp -r metrics/ios-render-test-runner $BASE_DIR/test-data/
cp -r metrics/ios-render-test-runner-style.json $BASE_DIR/test-data/
cp -r metrics/ios-render-test-runner-metrics.json $BASE_DIR/test-data/
cp -r metrics/cache-metrics.db $BASE_DIR/test-data/
cp -r metrics/cache-style.db $BASE_DIR/test-data/
