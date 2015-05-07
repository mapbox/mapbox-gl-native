#!/usr/bin/env bash

set -e
set -o pipefail

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S
