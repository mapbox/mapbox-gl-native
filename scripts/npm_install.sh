#!/usr/bin/env bash

set -e
set -o pipefail

cd bin
$1 install --clang=1
cd ../
