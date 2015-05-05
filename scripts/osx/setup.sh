#!/usr/bin/env bash

set -e
set -o pipefail

# Add Mason to PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S
