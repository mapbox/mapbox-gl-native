#!/usr/bin/env bash

set -e
set -o pipefail

cd bin
npm install --clang=1
cd ../
