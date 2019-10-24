#!/bin/bash

adb shell rm -rf /sdcard/render-test
adb shell mkdir /sdcard/render-test
adb shell mkdir /sdcard/render-test/vendor
adb shell mkdir /sdcard/render-test/ignores
adb shell mkdir /sdcard/render-test/render-test/tests

# push test sources
adb push ../../mapbox-gl-js/test/integration/render-tests /sdcard/render-test/mapbox-gl-js/test/integration/render-tests
adb push ../../mapbox-gl-js/test/integration/query-tests /sdcard/render-test/mapbox-gl-js/test/integration/query-tests 
adb push ../../mapbox-gl-js/test/integration/tiles /sdcard/render-test/mapbox-gl-js/test/integration/tiles
adb push ../../mapbox-gl-js/test/integration/glyphs /sdcard/render-test/mapbox-gl-js/test/integration/glyphs
adb push ../../mapbox-gl-js/test/integration/styles /sdcard/render-test/mapbox-gl-js/test/integration/styles 
adb push ../../mapbox-gl-js/test/integration/tilesets /sdcard/render-test/mapbox-gl-js/test/integration/tilesets 
adb push ../../mapbox-gl-js/test/integration/image /sdcard/render-test/mapbox-gl-js/test/integration/image 
adb push ../../mapbox-gl-js/test/integration/video /sdcard/render-test/mapbox-gl-js/test/integration/video 
adb push ../../vendor/mapbox-gl-styles/styles /sdcard/render-test/vendor/mapbox-gl-styles/styles 
adb push ../../vendor/mapbox-gl-styles/sprites /sdcard/render-test/vendor/mapbox-gl-styles/sprites 
adb push ../../mapbox-gl-js/test/integration/data /sdcard/render-test/mapbox-gl-js/test/integration/data 
adb push ../../mapbox-gl-js/test/integration/geojson /sdcard/render-test/mapbox-gl-js/test/integration/geojson
mkdir sprites
cp -r ../../mapbox-gl-js/test/integration/sprites/ sprites
adb push sprites /sdcard/render-test/mapbox-gl-js/test/integration/sprites
rm -rf sprites

# push extra expectations
adb push ../../render-test/expected/render-tests /sdcard/render-test/render-test/expected/render-tests

# push default ignore lists
adb shell mkdir /sdcard/render-test/platform
adb shell mkdir /sdcard/render-test/platform/node
adb shell mkdir /sdcard/render-test/platform/node/test
adb push ../../platform/node/test/ignores.json /sdcard/render-test/platform/node/test
adb shell mkdir /sdcard/render-test/render-test
adb push ../linux-ignores.json /sdcard/render-test/render-test
adb push ../tests/should-fail.json /sdcard/render-test/render-test/tests

# push manifest
adb push ../android-manifest.json /sdcard/render-test

adb shell ls /sdcard/render-test/