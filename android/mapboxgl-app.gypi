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
        '<@(boost_cflags)',
      ],
      'libraries': [
          '<@(openssl_static_libs)',
          '<@(libcurl_static_libs)',
          '<@(libpng_static_libs)',
          '<@(jpeg_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(libuv_static_libs)',
          '<@(nunicode_static_libs)',
          '<@(libzip_static_libs)',
      ],
      'variables': {
        'ldflags': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
          '<@(libpng_ldflags)',
          '<@(jpeg_ldflags)',
          '<@(sqlite_ldflags)',
          '<@(openssl_ldflags)',
          '<@(libcurl_ldflags)',
          '<@(zlib_ldflags)',
          '<@(libzip_ldflags)',
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
          'files': [
            '../common/ca-bundle.crt',
            '../styles/styles'
          ],
          'destination': '<(pwd)/../android/java/MapboxGLAndroidSDK/src/main/assets'
        },
      ],

      'actions': [
        {
          'action_name': 'Strip dynamic library',
          'inputs': [ '<(PRODUCT_DIR)/lib.target/libmapbox-gl.so' ],
          'outputs': [ '<(pwd)/../android/java/MapboxGLAndroidSDK/src/main/jniLibs/$(JNIDIR)/libmapbox-gl.so' ],
          'action': [ '$(STRIP)', '<@(_inputs)', '-o', '<@(_outputs)' ]
        },
      ],
    },
  ],
}
