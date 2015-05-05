#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

export MASON_PLATFORM=android
export MASON_ANDROID_ABI=${ANDROID_ABI}

mapbox_time "android_toolchain" \
./scripts/android/toolchain.sh
