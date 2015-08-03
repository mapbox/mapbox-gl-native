{
  'targets': [
    { 'target_name': 'asset-zip',
      'product_name': 'mbgl-asset-zip',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/asset_request_zip.cpp',
        '../platform/default/uv_zip.c',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'variables': {
        'cflags': [
          '<@(libuv_cflags)',
          '<@(libzip_cflags)',
        ],
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(libzip_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(libuv_ldflags)',
          '<@(libzip_ldflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(libzip_static_libs)',
        ],
        'defines': [
          '-DMBGL_ASSET_ZIP'
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [ '<@(cflags)' ],
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags': [ '<@(cflags)' ],
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],
      ],

      'direct_dependent_settings': {
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_CFLAGS': [ '<@(defines)' ],
              'OTHER_CPLUSPLUSFLAGS': [ '<@(defines)' ],
            }
          }, {
            'cflags': [ '<@(defines)' ],
            'cflags_cc': [ '<@(defines)' ],
          }]
        ],
      },

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
  ],
}
