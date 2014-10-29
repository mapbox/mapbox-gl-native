{
  'targets': [
    { 'target_name': 'mbgl-core',
      'product_name': 'mbgl-core',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
          'shaders',
      ],
      'variables': {
        'cflags_cc': [
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '<@(sqlite3_cflags)',
          '<@(zlib_cflags)',
          '-I<(boost_root)/include',
        ],
        'cflags': [
          '<@(uv_cflags)',
        ],
        'ldflags': [
          '<@(png_ldflags)',
          '<@(uv_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(zlib_ldflags)',
        ],
      },
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(test -f "config/constants_local.cpp" && echo "config/constants_local.cpp" || echo "config/constants.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'include_dirs': [
        '../include',
      ],
      'link_settings': {
        'libraries': [
          '<@(png_static_libs)',
          '<@(uv_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(zlib_static_libs)',
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
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [ '<@(ldflags)' ]
            }
          }, {
            'ldflags': [ '<@(ldflags)' ]
          }]
        ]
      }
    }
  ]
}
