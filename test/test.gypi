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
          '<@(earcut_cflags)'
        ],
        'ldflags': [
          '-landroid',
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
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          'include',
        ],

      },
    },
  ]
}
