{
  'targets': [
    { 'target_name': 'mbgl-osx',
      'product_name': 'mbgl-osx',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'sources': [
        '../platform/osx/cache_database_application_support.mm',
        '../platform/osx/shader_cache_application_support.mm',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/http_request_baton_cocoa.mm',
        '../platform/darwin/image.mm',
      ],
      'include_dirs': [
        '../include',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(uv_cflags)' ],
      },
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '-framework ImageIO',
            '-framework CoreServices',
          ],
        },
      },
    },
  ],
}
