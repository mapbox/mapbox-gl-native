#!/bin/bash

PREFIX=$1
PLATFORM=$2

VARIABLES="#!/bin/bash

## variables

CONFIG_MBGL_PREFIX=$PREFIX
CONFIG_MBGL_PLATFORM=$PLATFORM
"

echo "$VARIABLES" | cat - utils/mbgl-config/mbgl-config.template.sh \
    > $PREFIX/bin/mbgl-config

chmod 755 $PREFIX/bin/mbgl-config
