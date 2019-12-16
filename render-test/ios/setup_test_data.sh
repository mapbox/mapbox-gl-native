#!/bin/bash
rm -rf ../../test-data
mkdir ../../test-data
mkdir ../../test-data/integration

cp -r ../../mapbox-gl-js/test/integration/render-tests ../../test-data/integration/
cp -r ../../mapbox-gl-js/test/integration/query-tests ../../test-data/integration/
cp -r ../../metrics/expectations ../../test-data/
cp -r ../../metrics/ignores ../../test-data/
# cp -r ../../metrics/tests/
# cp -r ../../metrics/next-android-render-test-runner/
cp -r ../../metrics/next-ios-render-test-runner-style.json ../../test-data/
cp -r ../../metrics/cache-metrics.db ../../test-data/
cp -r ../../metrics/cache-style.db ../../test-data/