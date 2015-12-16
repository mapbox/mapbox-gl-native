{
  'targets': [
    { 'target_name': 'http-android',
      'product_name': 'mbgl-http-android',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/android/src/http_request_android.cpp',
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
          '-DMBGL_HTTP_ANDROID'
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
