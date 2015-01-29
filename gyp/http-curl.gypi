{
  'targets': [
    { 'target_name': 'http-curl',
      'product_name': 'mbgl-http-curl',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/http_request_curl.cpp',
      ],

      'include_dirs': [
        '../include',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(curl_cflags)',
          '-I<(boost_root)/include',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(curl_ldflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
          '<@(curl_static_libs)',
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
