#!/usr/bin/env bash

set -e
set -o pipefail

git clone git@github.com:creationix/nvm.git ../.nvm
source ../.nvm/nvm.sh
nvm install $NODE_NVM_VERSION
nvm use $NODE_NVM_VERSION
node --version
npm --version
