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
${CXX} --version

# Ensure mason is on the PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Start the mock X server
if [ -f /etc/init.d/xvfb ] ; then
    mapbox_time "start_xvfb" \
    sh -e /etc/init.d/xvfb start
    sleep 2 # sometimes, xvfb takes some time to start up
fi

# Make sure we're connecting to xvfb
export DISPLAY=:99.0

mapbox_time "checkout_mason" \
git submodule update --init .mason

# Touch package.json so that we are definitely going to run an npm update action
mapbox_time "touch_package_json" \
touch package.json

# Install and set up to load a more recent version of mesa
mapbox_time "install_mesa" \
mason install mesa 11.2.2
export LD_LIBRARY_PATH="`mason prefix mesa 11.2.2`/lib:${LD_LIBRARY_PATH:-}"

# Install and set up to load awscli
pip install --user awscli
export PATH="`python -m site --user-base`/bin:${PATH}"

# Install coveralls gem
gem install coveralls-lcov --no-rdoc --no-ri
