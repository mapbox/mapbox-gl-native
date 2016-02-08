{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'mbgl-offline',
      'product_name': 'mbgl-offline',
      'type': 'executable',

      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:headless-<(headless_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
        'mbgl.gyp:copy_certificate_bundle',
      ],

      'include_dirs': [
        '../src',
      ],

      'sources': [
        './offline.cpp',
      ],

      'variables' : {
        'cflags_cc': [
          '<@(boost_cflags)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
        }]
      ],
    },
  ],
}
