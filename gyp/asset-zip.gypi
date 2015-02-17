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
      ],

      'variables': {
        'cflags': [
          '<@(uv_cflags)',
          '<@(zip_cflags)',
        ],
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(zip_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(zip_ldflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
          '<@(zip_static_libs)',
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
