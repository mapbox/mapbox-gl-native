{
  'targets': [
    { 'target_name': 'mbgl-osx',
      'product_name': 'mbgl-osx',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],
      'sources': [
        '../platform/osx/shader_cache_application_support.mm',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/http_request_cocoa.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/image.mm',
        '../platform/default/asset_request_libuv.cpp',
      ],
      'include_dirs': [
        '../include',
        '../src',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ],
      },
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '-framework Foundation',
            '-framework ImageIO',
            '-framework CoreServices',
          ],
        },
      },
    },
  ],
}
