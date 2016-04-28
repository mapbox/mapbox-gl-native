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
  'conditions': [
    ['<(qt_version_major) == 5', {
      'includes': [ 'qmlapp/qquickmapboxgl.gypi' ],
    }],
  ],
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
        'qt.gypi',
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
        'include/qquickmapboxgl.hpp',
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
        'src/qquickmapboxgl.cpp',
        'src/qquickmapboxglrenderer.cpp',
        'src/qquickmapboxglrenderer.hpp',
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
          '<@(qt_core_cflags)',
          '<@(qt_gui_cflags)',
          '<@(qt_network_cflags)',
          '<@(rapidjson_cflags)',
          '<@(sqlite_cflags)',
          '<@(variant_cflags)',
          '<@(webp_cflags)',
          '-fPIC',
        ],
        'ldflags': [
          '<@(libjpeg-turbo_ldflags)',
          '<@(nunicode_ldflags)',
          '<@(opengl_ldflags)',
          '<@(qt_core_ldflags)',
          '<@(qt_gui_ldflags)',
          '<@(qt_network_ldflags)',
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
        ['<(qt_version_major) == 4', {
          'variables': {
            'cflags': [
              # Qt4 generates code with unused variables.
              '-Wno-unused-variable',
            ],
          },
        }],
        ['<(qt_version_major) == 5', {
          'variables': {
            'cflags': [
              '<@(qt_positioning_cflags)',
              '<@(qt_quick_cflags)',
            ],
            'ldflags': [
              '<@(qt_positioning_ldflags)',
              '<@(qt_quick_ldflags)',
            ],
          },
        }, {
          'sources/': [ [ 'exclude', 'qquick*' ] ],
        }],
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
            'xcode_settings': {
              'OTHER_LDFLAGS': [ '-all_load' ],
              'LD_DYLIB_INSTALL_NAME': '@executable_path/libqmapboxgl.so.<(qtlibversion)',
            }
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
