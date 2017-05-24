#!/usr/bin/env bash
# This script is sourced; do not set -e or -o pipefail here.

# Touch package.json so that we are definitely going to run an npm update action
touch package.json

function mapbox_install_logbt {
    export PATH=$(scripts/mason.sh PREFIX gdb VERSION 7.12)/bin:${PATH}
    curl -sSfL https://github.com/mapbox/logbt/archive/v2.0.1.tar.gz | tar --gunzip --extract --strip-components=2 --exclude="*md" --exclude="test*" --directory=.
    ./logbt --test
}

export -f mapbox_install_logbt

function mapbox_install_apitrace {
    export PATH=$(scripts/mason.sh PREFIX apitrace VERSION 6a30de1)/bin:${PATH}
}

export -f mapbox_install_apitrace

function mapbox_export_mesa_library_path {
    # Install and set up to load a more recent version of mesa
    MESA_PREFIX=$(scripts/mason.sh PREFIX mesa VERSION 13.0.4)
    export LD_LIBRARY_PATH="${MESA_PREFIX}/lib:${LD_LIBRARY_PATH:-}"
    export LIBGL_DRIVERS_PATH="${MESA_PREFIX}/lib/dri"
}

export -f mapbox_export_mesa_library_path

# Install and set up to load awscli
pip install --user awscli
export PATH="`python -m site --user-base`/bin:${PATH}"

# Install coveralls gem
gem install coveralls-lcov --no-rdoc --no-ri
