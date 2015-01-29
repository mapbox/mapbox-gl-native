#!/usr/bin/env bash

set -e
set -o pipefail

mkdir -p android/java/lib/src/main/jniLibs/${JNIDIR}
cp build/android/out/${BUILDTYPE}/lib.target/libmapbox-gl.so android/java/lib/src/main/jniLibs/${JNIDIR}/libmapbox-gl.so
${STRIP} --strip-unneeded android/java/lib/src/main/jniLibs/${JNIDIR}/libmapbox-gl.so
mkdir -p android/java/lib/src/main/assets
cp build/android/out/${BUILDTYPE}/ca-bundle.crt android/java/lib/src/main/assets/ca-bundle.crt
cp -r build/android/out/${BUILDTYPE}/styles android/java/lib/src/main/assets/styles
