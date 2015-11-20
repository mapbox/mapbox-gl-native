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
          '<@(boost_cflags)',
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
    },
  ],
}
