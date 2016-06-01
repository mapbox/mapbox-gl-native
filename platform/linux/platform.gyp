{
  'variables': {
    'loop_lib': 'uv',
    'headless_lib': 'glx',
    'coverage': '<!(echo $ENABLE_COVERAGE)>',
  },
  'conditions': [
    ['OS == "mac"', {
      'variables': {
        'headless_lib': 'cgl',
      }
    }],
  ],
  'includes': [
    '../../mbgl.gypi',
    '../../test/test.gypi',
    '../../benchmark/benchmark.gypi',
    '../../bin/glfw.gypi',
    '../../bin/render.gypi',
    '../../bin/offline.gypi',
  ],
  'targets': [
    {
      'target_name': 'test',
      'type': 'executable',

      'dependencies': [
        'test-lib',
        'platform-lib',
        'copy_certificate_bundle',
      ],

      'sources': [
        '../../test/src/main.cpp',
      ],
    },
    {
      'target_name': 'benchmark',
      'type': 'executable',

      'dependencies': [
        'benchmark-lib',
        'platform-lib',
      ],

      'sources': [
        '../../benchmark/src/main.cpp',
      ],
    },
    {
      'target_name': 'platform-lib',
      'product_name': 'mbgl-platform-linux',
      'type': 'static_library',
      'standalone_static_library': 1,
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
        '../default/log_stderr.cpp',
        '../default/string_stdlib.cpp',
        '../default/thread.cpp',
        '../default/image.cpp',
        '../default/webp_reader.cpp',
        '../default/png_reader.cpp',
        '../default/jpeg_reader.cpp',
        '../default/asset_file_source.cpp',
        '../default/http_file_source.cpp',
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
        '<@(nunicode_cflags)',
        '<@(sqlite_cflags)',
        '<@(rapidjson_cflags)',
        '<@(libcurl_cflags)',
        '<@(libpng_cflags)',
        '<@(libjpeg-turbo_cflags)',
        '<@(webp_cflags)',
      ],

      'link_settings': {
        'libraries': [
          '<@(nunicode_static_libs)',
          '<@(nunicode_ldflags)',
          '<@(sqlite_static_libs)',
          '<@(sqlite_ldflags)',
          '<@(zlib_static_libs)',
          '<@(zlib_ldflags)',
          '<@(libcurl_static_libs)',
          '<@(libcurl_ldflags)',
          '<@(libpng_static_libs)',
          '<@(libpng_ldflags)',
          '<@(libjpeg-turbo_static_libs)',
          '<@(libjpeg-turbo_ldflags)',
          '<@(webp_static_libs)',
          '<@(webp_ldflags)',
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
