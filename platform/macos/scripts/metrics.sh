#!/usr/bin/env bash

set -e
set -o pipefail

# Track individual architectures
scripts/log_binary_size.sh "build/macos/pkg/Mapbox.framework/Versions/Current/Mapbox"  "Platform=macOS,Arch=x86_64"
