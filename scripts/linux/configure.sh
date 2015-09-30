#!/usr/bin/env bash

BOOST_VERSION=1.57.0
BOOST_LIBPROGRAM_OPTIONS_VERSION=1.57.0
LIBCURL_VERSION=system
GLFW_VERSION=3.1
LIBPNG_VERSION=1.6.16
JPEG_VERSION=v9a
SQLITE_VERSION=3.8.8.1
LIBUV_VERSION=0.10.28
ZLIB_VERSION=system
NUNICODE_VERSION=1.5.1
LIBZIP_VERSION=0.11.2
VARIANT_VERSION=1.0

function print_opengl_flags {
    CONFIG+="    'opengl_cflags%': $(quote_flags $(pkg-config gl x11 --cflags)),"$LN
    CONFIG+="    'opengl_ldflags%': $(quote_flags $(pkg-config gl x11 --libs)),"$LN
}
