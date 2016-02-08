{
  'targets': [
    { 'target_name': 'core',
      'product_name': 'mbgl-core',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'shaders',
        'version',
      ],

      'sources': [
        '<!@(find ../src -name "*.hpp")',
        '<!@(find ../src -name "*.cpp")',
        '<!@(find ../src -name "*.c")',
        '<!@(find ../src -name "*.h")',
        '<!@(find ../include -name "*.hpp")',
        '<!@(find ../include -name "*.h")',
        '<!@(find ../src -name "*.glsl")',
        '../bin/style.json'
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'variables': {
        'cflags_cc': [
          '<@(opengl_cflags)',
          '<@(boost_cflags)',
          '<@(geojsonvt_cflags)',
          '<@(rapidjson_cflags)',
        ],
        'cflags': [
          '<@(opengl_cflags)',
          '<@(rapidjson_cflags)',
          '-fPIC'
        ],
        'ldflags': [
          '<@(opengl_ldflags)',
        ],
        'libraries': [
          '<@(geojsonvt_static_libs)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
          },
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'cflags': [ '<@(cflags)' ],
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
  ]
}
