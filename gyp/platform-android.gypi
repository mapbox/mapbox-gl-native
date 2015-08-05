{
  'targets': [
    { 'target_name': 'platform-android',
      'product_name': 'mbgl-platform-android',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/android/log_android.cpp',
        '../platform/android/asset_root.cpp',
        '../platform/default/thread.cpp',
        '../platform/default/string_stdlib.cpp',
        '../platform/default/image.cpp',
        '../platform/default/image_reader.cpp',
        '../platform/default/png_reader.cpp',
        '../platform/default/jpeg_reader.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libpng_cflags)',
          '<@(jpeg_cflags)',
          '<@(libuv_cflags)',
          '<@(nunicode_cflags)',
          '<@(boost_cflags)',
        ],
        'ldflags': [
          '<@(libpng_ldflags)',
          '<@(jpeg_ldflags)',
          '<@(libuv_ldflags)',
          '<@(nunicode_ldflags)',
        ],
        'libraries': [
          '<@(libpng_static_libs)',
          '<@(jpeg_static_libs)',
          '<@(libuv_static_libs)',
          '<@(nunicode_static_libs)',
        ],
      },

      'include_dirs': [
        '../include',
        '../src',
      ],

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
        }]
      ],

      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
      },
    },
  ],
}
