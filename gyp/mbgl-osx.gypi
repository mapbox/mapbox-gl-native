{
  'targets': [
    { 'target_name': 'mbgl-osx',
      'product_name': 'mbgl-osx',
      'type': 'static_library',
      'hard_dependency': 1,
      'sources': [
        '../platform/osx/cache_database_application_support.mm',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/http_request_baton_cocoa.mm',
      ],
      'include_dirs': [
        '../include',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(uv_cflags)' ],
      },
      'libraries': [
        '<@(uv_static_libs)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(uv_ldflags)' ],
        },
      },
    },
  ],
}
