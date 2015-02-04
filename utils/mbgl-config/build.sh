#!/bin/bash

PREFIX=$1
LDFLAGS=$2

VARIABLES="#!/bin/bash

## variables

CONFIG_MBGL_PREFIX=$PREFIX
CONFIG_MBGL_LDFLAGS=\"`cat $LDFLAGS`\"
"

echo "$VARIABLES" | cat - utils/mbgl-config/mbgl-config.template.sh \
    > $PREFIX/bin/mbgl-config

chmod 755 $PREFIX/bin/mbgl-config
