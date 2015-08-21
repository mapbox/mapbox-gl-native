{
  'targets': [
    { 'target_name': 'asset-fs',
      'product_name': 'mbgl-asset-fs',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/asset_request_fs.cpp',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(libuv_ldflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
        ],
        'defines': [
          '-DMBGL_ASSET_FS'
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
