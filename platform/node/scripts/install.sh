#!/usr/bin/env bash

set -e
set -o pipefail

git submodule update --init .mason

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

if [ ${TRAVIS_OS_NAME} == "linux" ]; then
    mason install mesa 10.4.3
fi

if [ ! -d ~/.nvm ]; then
    curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.28.0/install.sh | bash
fi

source ~/.nvm/nvm.sh

nvm install $NODE_VERSION
nvm alias default $NODE_VERSION

node --version
npm --version
