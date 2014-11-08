{
  'targets': [
    { 'target_name': 'mbgl-linux',
      'product_name': 'mbgl-linux',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'variables': {
        'cflags_cc': [
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '<@(curl_cflags)',
          '<@(nu_cflags)',
          '-I<(boost_root)/include',
        ],
        'cflags': [
          '<@(uv_cflags)',
          '<@(nu_cflags)',
        ],
        'ldflags': [
          '<@(png_ldflags)',
          '<@(uv_ldflags)',
          '<@(curl_ldflags)',
          '<@(nu_ldflags)',
        ],
      },
      'sources': [
        '../platform/default/cache_database_tmp.cpp',
        '../platform/default/log_stderr.cpp',
        '../platform/default/string_stdlib.cpp',
        '../platform/default/http_request_baton_curl.cpp',
        '../platform/default/image.cpp',
      ],
      'include_dirs': [
        '../include',
      ],
      'link_settings': {
        'libraries': [
          '<@(png_static_libs)',
        ],
      },
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
          }
        }, {
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
