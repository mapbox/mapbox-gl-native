#!/usr/bin/env bash

set -e
set -o pipefail

# Track individual architectures
scripts/check_binary_size.js "build/macos/pkg/Mapbox.framework/Versions/Current/Mapbox"  "macOS x86_64"
