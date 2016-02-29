{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'android-lib',
      'product_name': 'mapbox-gl',
      'type': 'shared_library',
      'hard_dependency': 1,

      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
      ],

      'include_dirs': [
        '../src',
      ],

      'sources': [
        './src/native_map_view.cpp',
        './src/jni.cpp',
      ],

      'cflags_cc': [
        '<@(boost_cflags)',
        '<@(variant_cflags)',
        '<@(jni.hpp_cflags)',
      ],
      'libraries': [
          '<@(libpng_static_libs)',
          '<@(libjpeg-turbo_static_libs)',
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
          '<@(libjpeg-turbo_ldflags)',
          '<@(sqlite_ldflags)',
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

    { 'target_name': 'example-custom-layer-lib',
      'product_name': 'example-custom-layer',
      'type': 'shared_library',
      'hard_dependency': 1,

      'sources': [
        './src/example_custom_layer.cpp',
      ],

      'include_dirs': [
        '../../include',
      ],

      'variables': {
        'ldflags': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
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

      'dependencies': [
        'android-lib',
        'example-custom-layer-lib',
      ],

      'variables': {
        'pwd': '<!(pwd)',
      },

      'copies': [
        {
          'files': [
            '../../common/ca-bundle.crt',
            '../../platform/default/resources/api_mapbox_com-digicert.der',
            '../../platform/default/resources/api_mapbox_com-geotrust.der',
            '../../platform/default/resources/star_tilestream_net.der',
          ],
          'destination': '<(pwd)/../platform/android/MapboxGLAndroidSDK/src/main/assets'
        },
        {
        'files': [
          '<(PRODUCT_DIR)/obj.target'
        ],
        'destination': '<(pwd)/../platform/android/MapboxGLAndroidSDK/src/main'
        },
      ],

      'actions': [
        {
          'action_name': 'Strip mapbox library',
          'inputs': [ '<(PRODUCT_DIR)/lib.target/libmapbox-gl.so' ],
          'outputs': [ '<(pwd)/../platform/android/MapboxGLAndroidSDK/src/main/jniLibs/$(JNIDIR)/libmapbox-gl.so' ],
          'action': [ '$(STRIP)', '<@(_inputs)', '-o', '<@(_outputs)' ]
        },
        {
          'action_name': 'Strip example custom layer library',
          'inputs': [ '<(PRODUCT_DIR)/lib.target/libexample-custom-layer.so' ],
          'outputs': [ '<(pwd)/../platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs/$(JNIDIR)/libexample-custom-layer.so' ],
          'action': [ '$(STRIP)', '<@(_inputs)', '-o', '<@(_outputs)' ]
        }
      ],
    },
  ],
}
