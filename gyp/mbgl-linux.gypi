{
  'targets': [
    { 'target_name': 'mbgl-linux',
      'product_name': 'mbgl-linux',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(curl_cflags)',
        ],
        'cflags': [
          '<@(uv_cflags)',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(curl_ldflags)',
        ],
      },
      'sources': [
        '../platform/default/cache_database_tmp.cpp',
        '../platform/default/log_stderr.cpp',
        '../platform/default/string_stdlib.cpp',
        '../platform/default/http_request_baton_curl.cpp',
      ],
      'include_dirs': [
        '../include',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
          }
        }, {
          'ldflags': [ '<@(ldflags)' ],
          'cflags_cc': [ '<@(cflags_cc)' ],
          'cflags': [ '<@(cflags)' ],
        }]
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [ '<@(ldflags)' ],
            }
          }, {
            'ldflags': [ '<@(ldflags)' ],
          }]
        ],
      },
    },
  ],
}
