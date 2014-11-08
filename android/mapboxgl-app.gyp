{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'androidapp',
      'product_name': 'mapbox-gl',
      'type': 'shared_library',
      'sources': [
        './cpp/NativeMapView.cpp',
        './cpp/JNI.cpp',
      ],
      'cflags_cc': [
        '-I<(boost_root)/include',
      ],
      'libraries': [
          '<@(openssl_static_libs)',
          '<@(curl_static_libs)',
          '<@(png_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(uv_static_libs)',
          '<@(nu_static_libs)',
      ],
      'variables': {
        'ldflags': [
          '-llog -landroid -lEGL -lGLESv2 -lstdc++ -latomic '
          '<@(png_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(openssl_ldflags)',
          '<@(curl_ldflags)',
          '<@(zlib_ldflags)',
        ],
      },
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
      ],
      'dependencies': [
        '../mapboxgl.gyp:mbgl-standalone',
        '../mapboxgl.gyp:mbgl-android',
        '../mapboxgl.gyp:copy_certificate_bundle',
      ],
      'copies': [{
        'files': [ '../styles' ],
        'destination': '<(PRODUCT_DIR)'
      }],
    },
  ],
}
