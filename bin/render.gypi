{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'mbgl-render',
      'product_name': 'mbgl-render',
      'type': 'executable',

      'dependencies': [
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:headless-<(headless_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
        '../mbgl.gyp:copy_certificate_bundle',
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
        ],
        'libraries': [
          '<@(glfw3_static_libs)',
          '<@(uv_static_libs)',
          '<@(boost_program_options_static_libs)'
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
