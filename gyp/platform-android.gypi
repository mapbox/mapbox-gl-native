{
  'targets': [
    { 'target_name': 'platform-android',
      'product_name': 'mbgl-platform-android',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/android/src/log_android.cpp',
        '../platform/default/async_task.cpp',
        '../platform/default/run_loop.cpp',
        '../platform/default/thread.cpp',
        '../platform/default/string_stdlib.cpp',
        '../platform/default/image.cpp',
        '../platform/default/png_reader.cpp',
        '../platform/default/jpeg_reader.cpp',
        '../platform/default/timer.cpp',
        '../platform/default/default_file_source.cpp',
        '../platform/default/online_file_source.cpp',
        '../platform/default/mbgl/storage/offline.hpp',
        '../platform/default/mbgl/storage/offline.cpp',
        '../platform/default/mbgl/storage/offline_database.hpp',
        '../platform/default/mbgl/storage/offline_database.cpp',
        '../platform/default/mbgl/storage/offline_download.hpp',
        '../platform/default/mbgl/storage/offline_download.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libpng_cflags)',
          '<@(libjpeg-turbo_cflags)',
          '<@(libuv_cflags)',
          '<@(nunicode_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(rapidjson_cflags)',
        ],
        'ldflags': [
          '<@(libpng_ldflags)',
          '<@(libjpeg-turbo_ldflags)',
          '<@(libuv_ldflags)',
          '<@(nunicode_ldflags)',
          '<@(sqlite_ldflags)',
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(libpng_static_libs)',
          '<@(libjpeg-turbo_static_libs)',
          '<@(libuv_static_libs)',
          '<@(nunicode_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
        ],
      },

      'include_dirs': [
        '../include',
        '../src',
        '../platform/default',
      ],

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
        }]
      ],

      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
      },
    },
  ],
}
