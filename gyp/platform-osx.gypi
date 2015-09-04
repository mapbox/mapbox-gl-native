{
  'targets': [
    { 'target_name': 'platform-osx',
      'product_name': 'mbgl-platform-osx',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/asset_root.mm',
        '../platform/darwin/image.mm',
        '../platform/darwin/nsthread.mm',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
        ],
        'ldflags': [
          '-framework Foundation',
          '-framework ImageIO',
          '-framework CoreServices',
          '-framework OpenGL',
          '-framework ApplicationServices',
        ],
      },

      'include_dirs': [
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
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        },
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
      },
    },
  ],
}
