#!/usr/bin/env bash

set -eu -o pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

find_framework() {
    step "Looking for Mapbox.framework…"
    echo $( find ./build -name Mapbox | tail -n 1 )
}

FRAMEWORK=$(find_framework)

if [[ -z "${FRAMEWORK}" || ! -f "${FRAMEWORK}" ]]; then
    echo "No framework found — building dynamic Mapbox.framework…"
    make iframework BUILD_DEVICE=false
    FRAMEWORK=$(find_framework)
fi

echo "Found framework: ${FRAMEWORK}"

step "Checking for un-namespaced symbols from mapbox-events-ios…"

# Symbols from mapbox-events-ios are prefixed MME. To avoid duplicate symbol
# warnings when multiple copes of mapbox-events-ios are included in a project,
# the maps SDK prefixes these symbols with MGL_.
SYMBOLS=$( nm "$FRAMEWORK" | grep \$_MME || true )

if [ -z "${SYMBOLS}" ]; then
    echo "✅ No un-namespaced symbols found."
else
    echo "❗️ Found un-namespaced symbols:"
    echo "${SYMBOLS}"
    exit 1
fi
