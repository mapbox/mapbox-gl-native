#!/usr/bin/env bash

set -o pipefail

cd bin
npm install --clang=1
cd ../
