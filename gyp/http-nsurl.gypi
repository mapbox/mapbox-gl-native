{
  'targets': [
    { 'target_name': 'http-nsurl',
      'product_name': 'mbgl-http-nsurl',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/darwin/http_request_nsurl.mm',
      ],

      'include_dirs': [
        '../include',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
        ],
        'ldflags': [
          '-framework Foundation', # For NSURLRequest
          '<@(uv_ldflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
        ],
      },

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'NO',
      },

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        },
      },
    },
  ],
}
