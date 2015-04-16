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
        '../src',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(curl_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(curl_ldflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
          '<@(curl_static_libs)',
        ],
        'defines': [
          '-DMBGL_HTTP_CURL'
        ],
      },

      'conditions': [
        ['host == "android"', {
          'variables': {
            # Android uses libzip and openssl to set CURL's CA bundle.
            'cflags_cc': [ '<@(zip_cflags)', '<@(openssl_cflags)' ],
            'ldflags': [ '<@(zip_ldflags)', ],
            'libraries': [ '<@(zip_static_libs)', ],
          },
        }],
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
