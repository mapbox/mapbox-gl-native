{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'mbgl-render',
      'product_name': 'mbgl-render',
      'type': 'executable',
      'sources': [
        './render.cpp',
      ],
      'variables' : {
        'cflags': [
          '<@(uv_cflags)',
          '<@(png_cflags)',
          '-I<(boost_root)/include',
        ],
        'ldflags': [
          '<@(glfw3_ldflags)',
          '<@(uv_ldflags)',
          '<@(sqlite3_static_libs)',
          '<@(sqlite3_ldflags)',
          '<@(curl_ldflags)',
          '<@(png_ldflags)',
          '<@(uv_static_libs)',
          '-L<(boost_root)/lib',
          '-lboost_program_options'
        ],
      },
      'conditions': [
        # add libuv include path and OpenGL libs
        ['OS == "mac"',
          {
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': ['<@(cflags)'],
              'OTHER_LDFLAGS': ['<@(ldflags)'],
            },
          },
          {
            'cflags': ['<@(cflags)'],
            'libraries': ['<@(ldflags)'],
          }],
      ],
      'include_dirs': [ '../src' ],
      'dependencies': [
        '../mapboxgl.gyp:mbgl-standalone',
        '../mapboxgl.gyp:mbgl-headless',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../mapboxgl.gyp:copy_certificate_bundle',
      ],
    },
  ],
}
