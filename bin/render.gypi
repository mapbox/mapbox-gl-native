{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'cli-deps',
      'type': 'none',

      'dependencies': [
        'core',
        'platform-<(platform_lib)',
        'headless-<(headless_lib)',
        'http-<(http_lib)',
        'asset-<(asset_lib)',
        'cache-<(cache_lib)',
        'copy_certificate_bundle',
      ],

      'export_dependent_settings': [
        'core',
        'platform-<(platform_lib)',
        'headless-<(headless_lib)',
        'http-<(http_lib)',
        'asset-<(asset_lib)',
        'cache-<(cache_lib)',
        'copy_certificate_bundle',
      ],

      'direct_dependent_settings': {
        'include_dirs': [
          '../src',
        ],

        'variables' : {
          'cflags_cc': [
            '<@(glfw3_cflags)',
            '<@(uv_cflags)',
            '<@(boost_cflags)',
          ],
          'ldflags': [
            '<@(glfw3_ldflags)',
            '<@(uv_ldflags)',
            '<@(boost_ldflags)',
            '-lboost_program_options'
          ],
          'libraries': [
            '<@(glfw3_static_libs)',
            '<@(uv_static_libs)',
          ],
        },

        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
              'OTHER_LDFLAGS': [ '<@(ldflags)' ],
            }
          }, {
            'cflags_cc': [ '<@(cflags_cc)' ],
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }]
        ],
      },
    },

    { 'target_name': 'mbgl-render',
      'product_name': 'mbgl-render',
      'type': 'executable',
      'dependencies': [ 'cli-deps' ],
      'sources': [ './render.cpp' ],
    },

    { 'target_name': 'mbgl-load-test',
      'product_name': 'mbgl-load-test',
      'type': 'executable',
      'dependencies': [ 'cli-deps' ],
      'sources': [ './load_test.cpp' ],
    },
  ],
}
