{
  'targets': [
    { 'target_name': 'platform-darwin',
      'product_name': 'mbgl-platform-darwin',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/darwin/src/mbgl/platform/application_root.mm',
        '../platform/darwin/src/mbgl/platform/asset_root.mm',
        '../platform/darwin/src/mbgl/platform/log_nslog.mm',
        '../platform/darwin/src/mbgl/platform/nsthread.mm',
        '../platform/darwin/include/mbgl/platform/reachability.h',
        '../platform/darwin/src/mbgl/platform/reachability.m',
        '../platform/darwin/src/mbgl/platform/settings_nsuserdefaults.mm',
        '../platform/darwin/src/mbgl/platform/string_nsstring.mm',
        '../platform/darwin/src/mbgl/util/image.mm',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
        ],
      },

      'include_dirs': [
        '../platform/darwin/include',
        '../include',
        '../src',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
      },

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
      },

      'all_dependent_settings': {
        'include_dirs': [
          '../platform/darwin/include',
        ],
      },
    },
  ],
}
