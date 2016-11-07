#!/usr/bin/env bash

set -e
set -o pipefail

# Generate stripped version
ARCH=$(uname -m)
RENDER=build/linux-${ARCH}/Release/mbgl-render
strip -s -x "${RENDER}" -o "${RENDER}-stripped"

# Track individual architecture
scripts/log_binary_size.sh "${RENDER}-stripped" "Platform=Linux,Compiler=${_CC},Arch=${ARCH}"
