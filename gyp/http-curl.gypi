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
          '<@(libuv_cflags)',
          '<@(libcurl_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(libuv_ldflags)',
          '<@(libcurl_ldflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(libcurl_static_libs)',
        ],
        'defines': [
          '-DMBGL_HTTP_CURL'
        ],
      },

      'conditions': [
        ['host == "android"', {
          'variables': {
            # Android uses libzip and openssl to set CURL's CA bundle.
            'cflags_cc': [ '<@(libzip_cflags)', '<@(openssl_cflags)' ],
            'ldflags': [ '<@(libzip_ldflags)', ],
            'libraries': [ '<@(libzip_static_libs)', ],
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
