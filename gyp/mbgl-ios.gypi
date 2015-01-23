{
  'targets': [
    { 'target_name': 'mbgl-ios',
      'product_name': 'mbgl-ios',
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
        '../platform/darwin/image.mm',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ],
        'libraries': [
          '<@(uv_static_libs)',
        ],
        'ldflags': [
          '-framework ImageIO',
          '-framework MobileCoreServices',
        ],
      },

      'include_dirs': [
        '../include',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
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
