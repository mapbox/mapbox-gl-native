#!/usr/bin/env bash

set -u

LIST=(
    "DIN%20Offc%20Pro%20Regular%2cArial%20Unicode%20MS%20Regular/0-255.pbf"
    "DIN%20Offc%20Pro%20Medium%2cArial%20Unicode%20MS%20Regular/0-255.pbf"
    "DIN%20Offc%20Pro%20Bold%2cArial%20Unicode%20MS%20Bold/0-255.pbf"
    "DIN%20Offc%20Pro%20Regular%2cArial%20Unicode%20MS%20Regular/256-511.pbf"
    "DIN%20Offc%20Pro%20Regular%2cArial%20Unicode%20MS%20Regular/8192-8447.pbf"
    "DIN%20Offc%20Pro%20Medium%2cArial%20Unicode%20MS%20Regular/8192-8447.pbf"
    "DIN%20Offc%20Pro%20Italic%2cArial%20Unicode%20MS%20Regular/0-255.pbf"

)

# from https://gist.github.com/cdown/1163649
urldecode() {
    local url_encoded="${1//+/ }"
    printf '%b' "${url_encoded//%/\x}"
}

for i in ${LIST[@]} ; do
    OUTPUT=`urldecode "$i"`
    if [ ! -f "${OUTPUT}" ] ; then
        mkdir -p "`dirname "${OUTPUT}"`"
        echo "Downloading glyph '${OUTPUT}'"
        curl -# "https://api.tiles.mapbox.com/v4/fontstack/${i}?access_token=${MAPBOX_ACCESS_TOKEN}" | gunzip > "${OUTPUT}"
    fi
done
