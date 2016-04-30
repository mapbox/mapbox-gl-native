{
  'targets': [
    {
      'target_name': 'mbgl-offline',
      'product_name': 'mbgl-offline',
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
        'offline.cpp',
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
