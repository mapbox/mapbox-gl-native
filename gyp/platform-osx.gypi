{
  'targets': [
    {
      'target_name': 'mapboxgl-osx',
      'product_name': 'mapboxgl-osx',
      'type': 'static_library',
      'include_dirs': [
        '../include',
      ],
      'sources': [
        '../platform/osx/cache_database_application_support.mm',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/http_request_baton_cocoa.mm',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(uv_cflags)',
        ],
      },
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '<@(uv_libraries)',
          ],
        },
      },
    },
  ],
}
