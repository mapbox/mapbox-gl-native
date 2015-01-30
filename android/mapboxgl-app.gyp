{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'android-lib',
      'product_name': 'mapbox-gl',
      'type': 'shared_library',
      'hard_dependency': 1,

      'dependencies': [
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        './cpp/native_map_view.cpp',
        './cpp/jni.cpp',
      ],

      'cflags_cc': [
        '-I<(boost_root)/include',
      ],
      'libraries': [
          '<@(openssl_static_libs)',
          '<@(curl_static_libs)',
          '<@(png_static_libs)',
          '<@(jpeg_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(uv_static_libs)',
          '<@(nu_static_libs)',
          '<@(zip_static_libs)',
      ],
      'variables': {
        'ldflags': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
          '<@(png_ldflags)',
          '<@(jpeg_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(openssl_ldflags)',
          '<@(curl_ldflags)',
          '<@(zlib_ldflags)',
          '<@(zip_ldflags)',
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
    },


    { 'target_name': 'androidapp',
      'type': 'none',
      'hard_dependency': 1,

      'variables': {
        'pwd': '<!(pwd)',
      },

      'copies': [
        {
          'files': [ '<(PRODUCT_DIR)/lib.target/libmapbox-gl.so' ],
          'destination': '<(pwd)/java/lib/src/main/jniLibs/$(JNIDIR)'
        },
        {
          'files': [ '../common/ca-bundle.crt' ],
          'destination': '<(pwd)/java/lib/src/main/assets'
        },
        {
          'files': [ '../styles/styles' ],
          'destination': '<(pwd)/java/lib/src/main/assets'
        },
      ],
    },
  ],
}
