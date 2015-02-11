{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'mbgl-render',
      'product_name': 'mbgl-render',
      'type': 'executable',

      'dependencies': [
        'core',
        'platform-<(platform_lib)',
        'headless-<(headless_lib)',
        'http-<(http_lib)',
        'asset-<(asset_lib)',
        'cache-<(cache_lib)',
        'copy_certificate_bundle',
      ],

      'include_dirs': [
        '../src',
      ],

      'sources': [
        './render.cpp',
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
  ],
}
