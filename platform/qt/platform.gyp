{
  'variables': {
    'loop_lib': 'qt',
    'conditions': [
      ['OS == "mac"', {
        'headless_lib': 'cgl',
      }, {
        'headless_lib': 'glx',
      }]
    ],
    'qtlibversion': '1.0.0',
  },
  'includes': [
    'app/qmapboxgl.gypi',
    '../../mbgl.gypi',
    '../../test/test.gypi',
  ],
  'targets': [
    {
      'target_name': 'platform-lib',
      'product_name': 'mbgl-platform-qt',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'dependencies': [
        'core',
      ],

      'includes': [
        '../../platform/qt/qt.gypi',
      ],

      'sources': [
        '../default/asset_file_source.cpp',
        '../default/default_file_source.cpp',
        '../default/jpeg_reader.cpp',
        '../default/log_stderr.cpp',
        '../default/mbgl/storage/offline.cpp',
        '../default/mbgl/storage/offline_database.cpp',
        '../default/mbgl/storage/offline_download.cpp',
        '../default/online_file_source.cpp',
        '../default/sqlite3.cpp',
        '../default/string_stdlib.cpp',
        '../default/thread.cpp',
        '../default/webp_reader.cpp',
        'include/qmapbox.hpp',
        'include/qmapboxgl.hpp',
        'qmapbox.qrc',
        'src/async_task.cpp',
        'src/async_task_impl.hpp',
        'src/http_file_source.cpp',
        'src/http_file_source.hpp',
        'src/http_request.cpp',
        'src/http_request.hpp',
        'src/image.cpp',
        'src/qmapbox.cpp',
        'src/qmapboxgl.cpp',
        'src/qmapboxgl_p.hpp',
        'src/run_loop.cpp',
        'src/run_loop_impl.hpp',
        'src/timer.cpp',
        'src/timer_impl.hpp',
      ],

      'variables': {
        'cflags': [
          '<@(boost_cflags)',
          '<@(libjpeg-turbo_cflags)',
          '<@(nunicode_cflags)',
          '<@(opengl_cflags)',
          '<@(qt_cflags)',
          '<@(rapidjson_cflags)',
          '<@(sqlite_cflags)',
          '<@(variant_cflags)',
          '<@(webp_cflags)',
          '-Wno-error', # TODO: eliminate
          '-fPIC',
        ],
        'ldflags': [
          '<@(libjpeg-turbo_ldflags)',
          '<@(nunicode_ldflags)',
          '<@(opengl_ldflags)',
          '<@(qt_ldflags)',
          '<@(sqlite_ldflags)',
          '<@(webp_ldflags)',
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(nunicode_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(libjpeg-turbo_static_libs)',
          '<@(webp_static_libs)',
          '<@(zlib_static_libs)',
        ],
      },

      'include_dirs': [
        'include',
        '../default',
        '../../include',
        '../../src', # TODO: eliminate
      ],

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags)' ],
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
    },
    {
      'target_name': 'qt-lib',
      'product_name': 'qmapboxgl',
      'type': 'shared_library',
      'product_extension': 'so.<(qtlibversion)',

      'includes': [
        'qt.gypi',
      ],

      'dependencies': [
        'platform-lib',
      ],

      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': { 'OTHER_LDFLAGS': [ '-all_load' ] }
          }]
        ],
      },
    },
    {
      'target_name': 'test',
      'type': 'executable',

      'dependencies': [
        'test-lib',
        'platform-lib',
      ],

      'sources': [
        '../../test/src/main.cpp',
      ],
    },
  ],
}
