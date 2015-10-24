#!/usr/bin/env bash

BOOST_VERSION=1.59.0
BOOST_LIBPROGRAM_OPTIONS_VERSION=1.59.0
LIBCURL_VERSION=system
GLFW_VERSION=3.1.2
LIBPNG_VERSION=1.6.18
JPEG_VERSION=v9a
SQLITE_VERSION=3.9.1
LIBUV_VERSION=1.7.5
ZLIB_VERSION=system
NUNICODE_VERSION=1.6
LIBZIP_VERSION=1.0.1
VARIANT_VERSION=1.0
RAPIDJSON_VERSION=1.0.2
GTEST_VERSION=1.7.0

function print_opengl_flags {
    CONFIG+="    'opengl_cflags%': $(quote_flags $(pkg-config gl x11 --cflags)),"$LN
    CONFIG+="    'opengl_ldflags%': $(quote_flags $(pkg-config gl x11 --libs)),"$LN
}
