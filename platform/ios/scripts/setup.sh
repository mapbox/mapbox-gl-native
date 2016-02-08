#!/usr/bin/env bash
# This script is sourced; do not set -e or -o pipefail here.

# Ensure mason is on the PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"
