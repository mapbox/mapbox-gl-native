{
  'targets': [
    {
      'target_name': 'test-lib',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        '../include',
        '../src',
        '../platform/default',
        'include',
        'src',
      ],

      'sources': [
        'algorithm/covered_by_children.cpp',
        'algorithm/generate_clip_ids.cpp',
        'algorithm/mock.hpp',
        'algorithm/update_renderables.cpp',

        'src/mbgl/test/stub_file_source.hpp',
        'src/mbgl/test/stub_file_source.cpp',
        'src/mbgl/test/util.hpp',
        'src/mbgl/test/util.cpp',
        'src/mbgl/test/fixture_log_observer.hpp',
        'src/mbgl/test/fixture_log_observer.cpp',
        'src/mbgl/test/test.cpp'
      ],

      'variables': {
        'cflags_cc': [
          '<@(gtest_cflags)',
          '<@(opengl_cflags)',
          '<@(protozero_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(geojsonvt_cflags)',
          '<@(rapidjson_cflags)',
          '<@(pixelmatch_cflags)',
          '<@(earcut_cflags)',
        ],
        'ldflags': [
          '<@(gtest_ldflags)',
          '<@(sqlite_ldflags)',
        ],
        'libraries': [
          '<@(gtest_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(geojsonvt_static_libs)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],
      ],
      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }],
          
          #Android doesn't support -pthread
          ['target_platform == "android"', {
            'libraries!': ['<@(gtest_ldflags)']
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          'include',
        ],
        
        # Force the linker to include all the objects from the lib-test archive. Otherwise they'd
        # be discarded because there are no undefined symbols to pull them in, and the resulting
        # executable would run zero tests.
        #
        # Don't do this for android as linking this in a shared library leads to linker
        # errors: "previous definition here"


        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-Wl,-force_load,<(PRODUCT_DIR)/libtest-lib.a',
              ],
            }
          }, {
            'conditions': [
              ['target_platform != "android"', {
                'link_settings': {
                  'ldflags': [
                    '-Wl,-whole-archive <(PRODUCT_DIR)/libtest-lib.a -Wl,-no-whole-archive',
                  ],
                },
              }]
            ],
          }],
        ]
      },
    },
  ]
}
