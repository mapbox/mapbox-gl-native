{
  'targets': [
    { 'target_name': 'mbgl-core',
      'product_name': 'mbgl-core',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
          'shaders',
          'version',
      ],
      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(sqlite3_cflags)',
          '<@(zlib_cflags)',
          '-I<(boost_root)/include',
        ],
        'cflags': [
          '<@(uv_cflags)','-fPIC'
        ],
      },
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'include_dirs': [
        '../include',
        '../src',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
            'SKIP_INSTALL': 'YES'
          },
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'cflags': [ '<@(cflags)' ],
        }]
      ]
    },
    {
      'target_name': 'mbgl-standalone',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
          'mbgl-core'
      ],
      'variables': {
        'core_lib':'<(PRODUCT_DIR)/libmbgl-core.a',
        'standalone_lib':'<(standalone_product_dir)/libmbgl.a'
      },
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [ '<(standalone_lib)' ],
            }
          }, {
            'ldflags': [ '<(standalone_lib)' ],
          }]
        ],
      },
      'actions': [
        {
          'action_name': 'build standalone core lib',
          'inputs': [
              '<(core_lib)'
          ],
          'outputs': [
              '<(standalone_lib)'
          ],
          'action': [
              './gyp/merge_static_libs.py',
              '<(standalone_lib)',
              '<@(uv_static_libs)',
              '<@(curl_static_libs)',
              '<@(sqlite3_static_libs)',
              '<(core_lib)'
          ],
        }
      ]
    }
  ]
}
