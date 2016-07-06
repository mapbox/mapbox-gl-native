#!/usr/bin/env bash

CXX11ABI=${CXX11ABI:-$(scripts/check-cxx11abi.sh)}

UNIQUE_RESOURCE_VERSION=dev
PROTOZERO_VERSION=1.3.0
BOOST_VERSION=1.60.0
GEOMETRY_VERSION=0.8.0
GEOJSON_VERSION=0.1.4${CXX11ABI:-}
GEOJSONVT_VERSION=6.1.0
GTEST_VERSION=1.7.0${CXX11ABI:-}
LIBJPEG_TURBO_VERSION=1.4.2
NUNICODE_VERSION=1.6
PIXELMATCH_VERSION=0.9.0
RAPIDJSON_VERSION=1.0.2
SQLITE_VERSION=3.9.1
VARIANT_VERSION=1.1.0
ZLIB_VERSION=system
WEBP_VERSION=0.5.0
EARCUT_VERSION=0.11

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

    QT_VERSION_MAJOR=$(qmake -query QT_VERSION | cut -d. -f1)
    CONFIG+="    'qt_version_major%': ['${QT_VERSION_MAJOR}'],"$LN
    CONFIG+="    'qt_image_decoders%': [0],"$LN

    CONFIG+="    'qt_core_cflags%': $(quote_flags $(mason cflags Qt system "QtCore")),"$LN
    CONFIG+="    'qt_gui_cflags%': $(quote_flags $(mason cflags Qt system "QtGui")),"$LN
    CONFIG+="    'qt_opengl_cflags%': $(quote_flags $(mason cflags Qt system "QtOpenGL")),"$LN
    CONFIG+="    'qt_network_cflags%': $(quote_flags $(mason cflags Qt system "QtNetwork")),"$LN

    CONFIG+="    'qt_core_ldflags%': $(quote_flags $(mason ldflags Qt system "QtCore")),"$LN
    CONFIG+="    'qt_gui_ldflags%': $(quote_flags $(mason ldflags Qt system "QtGui")),"$LN
    CONFIG+="    'qt_opengl_ldflags%': $(quote_flags $(mason ldflags Qt system "QtOpenGL")),"$LN
    CONFIG+="    'qt_network_ldflags%': $(quote_flags $(mason ldflags Qt system "QtNetwork")),"$LN

    if [ ${QT_VERSION_MAJOR} -gt 4 ]; then
        CONFIG+="    'qt_location_cflags%': $(quote_flags $(mason cflags Qt system "QtLocation")),"$LN
        CONFIG+="    'qt_qml_cflags%': $(quote_flags $(mason cflags Qt system "QtQml")), "$LN
        CONFIG+="    'qt_quick_cflags%': $(quote_flags $(mason cflags Qt system "QtQuick")), "$LN
        CONFIG+="    'qt_positioning_cflags%': $(quote_flags $(mason cflags Qt system "QtPositioning")),"$LN

        CONFIG+="    'qt_location_ldflags%': $(quote_flags $(mason ldflags Qt system "QtLocation")),"$LN
        CONFIG+="    'qt_qml_ldflags%': $(quote_flags $(mason ldflags Qt system "QtQml")), "$LN
        CONFIG+="    'qt_quick_ldflags%': $(quote_flags $(mason ldflags Qt system "QtQuick")), "$LN
        CONFIG+="    'qt_positioning_ldflags%': $(quote_flags $(mason ldflags Qt system "QtPositioning")),"$LN
    fi

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

CONFIGURE_SUPLATFORM=platform/qt/scripts/configure-${MASON_PLATFORM_VERSION}.sh

if [ -f $CONFIGURE_SUPLATFORM ]; then
   source $CONFIGURE_SUPLATFORM
fi
