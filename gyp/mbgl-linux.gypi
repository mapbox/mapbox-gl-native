{
  'targets': [
    { 'target_name': 'mbgl-linux',
      'product_name': 'mbgl-linux',
      'type': 'static_library',
      'hard_dependency': 1,
      'include_dirs': [
        '../include'
      ],
      'sources': [
        '../platform/default/cache_database_tmp.cpp',
        '../platform/default/log_stderr.cpp',
        '../platform/default/string_stdlib.cpp',
        '../platform/default/http_request_baton_curl.cpp',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [
              '<@(uv_cflags)',
              '<@(curl_cflags)',
            ],
          }
        }, {
          'cflags': [
            '<@(uv_cflags)',
            '<@(curl_cflags)',
          ],
        }]
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '<@(uv_libraries)',
                '<@(curl_libraries)',
              ]
            }
          }, {
            'libraries': [
              '<@(uv_libraries)',
              '<@(curl_libraries)',
              '-lboost_regex',
            ]
          }]
        ]
      }
    },
  ]
}
