#!/usr/bin/env bash

BOOST_VERSION=1.59.0
GEOJSONVT_VERSION=4.1.2
GTEST_VERSION=1.7.0
LIBJPEG_TURBO_VERSION=1.4.2
NUNICODE_VERSION=1.6
PIXELMATCH_VERSION=0.9.0
RAPIDJSON_VERSION=1.0.2
SQLITE_VERSION=3.9.1
VARIANT_VERSION=1.1.0
ZLIB_VERSION=system
WEBP_VERSION=0.5.0

function print_default_flags {
    CONFIG+="    'cflags': $(quote_flags -fvisibility=hidden),"$LN
}

if [ "$MASON_PLATFORM" == "osx" ]; then
    function print_opengl_flags {
        CONFIG+="    'opengl_cflags%': [],"$LN
        CONFIG+="    'opengl_ldflags%': ['-framework OpenGL', '-framework CoreFoundation'],"$LN
    }
else
    function print_opengl_flags {
        CONFIG+="    'opengl_cflags%': $(quote_flags $(pkg-config gl x11 --cflags)),"$LN
        CONFIG+="    'opengl_ldflags%': $(quote_flags $(pkg-config gl x11 --libs)),"$LN
    }
fi

function print_qt_flags {
    mason install Qt system

    CONFIG+="    'qt_cflags%': $(quote_flags $(mason cflags Qt system "QtCore QtGui QtOpenGL QtNetwork")),"$LN
    CONFIG+="    'qt_ldflags%': $(quote_flags $(mason ldflags Qt system "QtCore QtGui QtOpenGL QtNetwork")),"$LN

    QT_VERSION_MAJOR=$(qmake -query QT_VERSION | cut -d. -f1)
    if hash moc 2>/dev/null && hash rcc 2>/dev/null; then
        CONFIG+="    'qt_moc%': '$(which moc)',"$LN
        CONFIG+="    'qt_rcc%': '$(which rcc)',"$LN
    elif [ ${QT_VERSION_MAJOR} -gt 4 ] ; then
        CONFIG+="    'qt_moc%': '$(pkg-config Qt${QT_VERSION_MAJOR}Core --variable=host_bins)/moc',"$LN
        CONFIG+="    'qt_rcc%': '$(pkg-config Qt${QT_VERSION_MAJOR}Core --variable=host_bins)/rcc',"$LN
    else
        CONFIG+="    'qt_moc%': '$(pkg-config QtCore --variable=moc_location)',"$LN
        CONFIG+="    'qt_rcc%': '$(pkg-config QtCore --variable=rcc_location)',"$LN
    fi
}

CONFIGURE_SUPLATFORM=platform/qt/scripts/configure-${SUBPLATFORM}.sh

if [ -f $CONFIGURE_SUPLATFORM ]; then
   source $CONFIGURE_SUPLATFORM
fi
