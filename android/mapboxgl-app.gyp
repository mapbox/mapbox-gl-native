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
        './NativeMapView.cpp',
        './JNI.cpp',
      ],
      'cflags_cc': [
        '-I<(boost_root)/include',
      ],
      'variables': {
        'ldflags': [
          '-llog -landroid -lEGL -lGLESv2 '
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
      ],
    },
  ],
}
