#!/usr/bin/env bash

set -e
set -o pipefail

(cd ./test/suite/ && ./bin/compare_images.py native)
