{
  'variables': {
    'loop_lib': 'android',
    'headless_lib': 'egl',
    'coverage': 0,
  },
  'includes': [
    '../../mbgl.gypi',
    '../../test/test.gypi',
  ],
  'targets': [
    {
      'target_name': 'platform-lib',
      'product_name': 'mapbox-gl',
      'type': 'shared_library',
      'hard_dependency': 1,
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        '../default',
        '../../include',
        '../../src', # TODO: eliminate
      ],

      'sources': [
        'src/native_map_view.cpp',
        'src/jni.cpp',
        'src/attach_env.cpp',
        'src/log_android.cpp',
        'src/http_file_source.cpp',
        'src/asset_file_source.cpp',
        '../default/thread.cpp',
        '../default/string_stdlib.cpp',
        '../default/image.cpp',
        '../default/png_reader.cpp',
        '../default/jpeg_reader.cpp',
        '../default/default_file_source.cpp',
        '../default/online_file_source.cpp',
        '../default/mbgl/storage/offline.hpp',
        '../default/mbgl/storage/offline.cpp',
        '../default/mbgl/storage/offline_database.hpp',
        '../default/mbgl/storage/offline_database.cpp',
        '../default/mbgl/storage/offline_download.hpp',
        '../default/mbgl/storage/offline_download.cpp',
        '../default/sqlite3.hpp',
        '../default/sqlite3.cpp',
      ],

      'cflags_cc': [
        '<@(boost_cflags)',
        '<@(rapidjson_cflags)',
        '<@(nunicode_cflags)',
        '<@(sqlite_cflags)',
        '<@(jni.hpp_cflags)',
        '<@(libzip_cflags)',
        '<@(libpng_cflags)',
        '<@(libjpeg-turbo_cflags)',
      ],

      'link_settings': {
        'libraries': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
          '<@(nunicode_static_libs)',
          '<@(nunicode_ldflags)',
          '<@(sqlite_static_libs)',
          '<@(sqlite_ldflags)',
          '<@(zlib_static_libs)',
          '<@(zlib_ldflags)',
          '<@(libzip_static_libs)',
          '<@(libzip_ldflags)',
          '<@(libpng_static_libs)',
          '<@(libpng_ldflags)',
          '<@(libjpeg-turbo_static_libs)',
          '<@(libjpeg-turbo_ldflags)',
        ],
      },
    },
    {
      'target_name': 'example-custom-layer-lib',
      'product_name': 'example-custom-layer',
      'type': 'shared_library',
      'hard_dependency': 1,

      'sources': [
        './src/example_custom_layer.cpp',
      ],

      'include_dirs': [
        '../../include',
      ],

      'link_settings': {
        'libraries': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
        ],
      },
    },
    {
      'target_name': 'all',
      'type': 'none',
      'hard_dependency': 1,

      'dependencies': [
        'platform-lib',
        'example-custom-layer-lib',
	'test',
      ],

      'variables': {
        'pwd': '<!(pwd)',
      },

      'copies': [
        {
          'files': [
            '../../common/ca-bundle.crt',
            '../default/resources/api_mapbox_com-digicert.der',
            '../default/resources/api_mapbox_com-geotrust.der',
            '../default/resources/star_tilestream_net.der',
          ],
          'destination': '<(DEPTH)/platform/android/MapboxGLAndroidSDK/src/main/assets'
        },
      ],

      'actions': [
        {
          'action_name': 'Strip mapbox library',
          'inputs': [ '<(PRODUCT_DIR)/lib.target/libmapbox-gl.so' ],
          'outputs': [ '<(DEPTH)/platform/android/MapboxGLAndroidSDK/src/main/jniLibs/$(JNIDIR)/libmapbox-gl.so' ],
          'action': [ '$(STRIP)', '<@(_inputs)', '-o', '<@(_outputs)' ]
        },
        {
          'action_name': 'Strip example custom layer library',
          'inputs': [ '<(PRODUCT_DIR)/lib.target/libexample-custom-layer.so' ],
          'outputs': [ '<(DEPTH)/platform/android//MapboxGLAndroidSDKTestApp/src/main/jniLibs/$(JNIDIR)/libexample-custom-layer.so' ],
          'action': [ '$(STRIP)', '<@(_inputs)', '-o', '<@(_outputs)' ]
        }
      ],
    },
    {
      'target_name': 'test',
      'product_name': 'test-jni-lib',
      'type': 'shared_library',
      'hard_dependency': 1,

      'dependencies': [
        'test-lib',
        'platform-lib',
      ],

      'cflags_cc': [
        '<@(boost_cflags)',
        '<@(rapidjson_cflags)',
        '<@(nunicode_cflags)',
        '<@(sqlite_cflags)',
        '<@(jni.hpp_cflags)',
        '<@(libzip_cflags)',
        '<@(libpng_cflags)',
        '<@(libjpeg-turbo_cflags)',
      ],

      'link_settings': {
        'libraries': [
          '-llog',
          '-landroid',
          '-lEGL',
          '-lGLESv2',
          '-lstdc++',
          '-latomic',
        ],
      },

      'sources': [
        '../../test/src/main.jni.cpp',
      ],

      'include_dirs': [
        '../../include',
      ],
    },
  ],
}
