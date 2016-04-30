{
  'targets': [
    {
      'target_name': 'mbgl-render',
      'product_name': 'mbgl-render',
      'type': 'executable',

      'dependencies': [
        'core',
        'platform-lib',
        'copy_certificate_bundle',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'sources': [
        'render.cpp',
      ],

      'cflags_cc': [
        '<@(boost_cflags)',
      ],

      'link_settings': {
        'libraries': [
          '<@(boost_libprogram_options_static_libs)'
        ],
      },

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(boost_cflags)',
        ],
      }
    },
  ],
}
