#!/usr/bin/env bash

set -e
set -o pipefail

# Generate stripped version
ARCH=$(uname -m)
RENDER=build/linux-${ARCH}/Release/mbgl-render
strip -s -x "${RENDER}" -o "${RENDER}-stripped"

scripts/check_binary_size.js "${RENDER}-stripped" "Linux"
