#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

if [ ${TRAVIS_OS_NAME} == "linux" ]; then
    mapbox_time "install_mesa" \
    mason install mesa 10.4.3
fi

if [ ! -d ~/.nvm ]; then
    curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.29.0/install.sh | bash
fi

source ~/.nvm/nvm.sh

mapbox_time $NODE_VERSION \
nvm install $NODE_VERSION

nvm alias default $NODE_VERSION

node --version
npm --version
