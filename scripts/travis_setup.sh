#!/usr/bin/env bash
# This script is sourced; do not set -e or -o pipefail here.

if [ ! -z "${_CXX}" ]; then export CXX="${_CXX}" ; fi
if [ ! -z "${_CC}" ]; then export CC="${_CC}" ; fi

if [ "${CCACHE:-0}" -ge 1 ]; then
    export CXX="ccache ${CXX}"
    export CC="ccache ${CC}"

    # ccache splits up the compile steps, so we end up with unused arguments in some steps.
    # Clang also thinks that ccache isn't interactive, so we explicitly need to enable color.
    if [ $(echo | ${CXX} -dM -E - | grep -c "#define __clang__ 1") -ge 1 ]; then
        export CXX="${CXX} -Qunused-arguments -fcolor-diagnostics"
        export CC="${CC} -Qunused-arguments -fcolor-diagnostics"
    else
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60304
        # GCC normally throws this error which is in GTest, but *only* when compilation and
        # preprocessing aren't combined in one step. However, when using ccache with GCC, we are
        # running them in separate steps, so this warning/error is shown.
        export CXX="${CXX} -Wno-conversion-null"
        export CC="${CC} -Wno-conversion-null"
    fi
fi

echo "export CXX=\"${CXX}\""
echo "export CC=\"${CC}\""
if [ -x $(which ${CXX}) ]; then
    ${CXX} --version
fi

# Touch package.json so that we are definitely going to run an npm update action
mapbox_time "touch_package_json" \
touch package.json

function mapbox_install_logbt {
    scripts/mason.sh INSTALL gdb VERSION 7.12
    export PATH=$(scripts/mason.sh PREFIX gdb VERSION 7.12)/bin:${PATH}
    curl -sSfL https://github.com/mapbox/logbt/archive/v2.0.1.tar.gz | tar --gunzip --extract --strip-components=2 --exclude="*md" --exclude="test*" --directory=.
    sudo ./logbt --setup
    ./logbt --test
}

function mapbox_start_xvfb {
    if [ ! -f /etc/init.d/xvfb ]; then
        echo "Error: Could not start Xvfb mock server."
        exit 1
    fi

    mapbox_time "start_xvfb" \
    sh -e /etc/init.d/xvfb start
    sleep 2 # sometimes, xvfb takes some time to start up

    # Make sure we're connecting to xvfb
    export DISPLAY=:99.0
}

export -f mapbox_start_xvfb

function mapbox_export_mesa_library_path {
    # Install and set up to load a more recent version of mesa
    mapbox_time "install_mesa" \
    scripts/mason.sh install mesa VERSION 13.0.4

    MESA_PREFIX=`scripts/mason.sh PREFIX mesa VERSION 13.0.4`
    export LD_LIBRARY_PATH="${MESA_PREFIX}/lib:${LD_LIBRARY_PATH:-}"
    export LIBGL_DRIVERS_PATH="${MESA_PREFIX}/lib/dri"
}

export -f mapbox_export_mesa_library_path

# Install and set up to load awscli
pip install --user awscli
export PATH="`python -m site --user-base`/bin:${PATH}"

# Install coveralls gem
gem install coveralls-lcov --no-rdoc --no-ri
