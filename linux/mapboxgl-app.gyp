{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'linuxapp',
      'product_name': 'mapbox-gl',
      'type': 'executable',
      'sources': [
        './main.cpp',
        '../platform/default/settings_json.cpp',
        '../platform/default/glfw_view.cpp',
        '../platform/default/log_stderr.cpp',
      ],
      'libraries': [
        '<@(glfw3_static_libs)',
      ],
      'conditions': [
        ['OS == "mac"',

        # Mac OS X
        {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
              '<@(glfw3_cflags)',
            ],
          },
          'libraries': [
            '-L<(PRODUCT_DIR)/',
            '-lmbgl',
            '-lmbgl-linux',
            '<@(png_ldflags)',
            '<@(sqlite3_ldflags)',
            '<@(glfw3_ldflags)',
            '<@(curl_ldflags)',
            '<@(zlib_ldflags)',
          ]
        },
        # Non-Mac OS X
        {
          'cflags_cc': [
            '<@(glfw3_cflags)',
            '-I<(boost_root)/include',
          ],
          'libraries': [
            '-Wl,--start-group',
            '-L<(PRODUCT_DIR)/',
            '-lmbgl',
            '-lmbgl-linux',
            '<@(curl_static_libs)',
            '<@(png_ldflags)',
            '<@(sqlite3_ldflags)',
            '<@(glfw3_ldflags)',
            '-L<(boost_root)/lib',
            '-lboost_regex',
            '<@(curl_ldflags)',
            '<@(zlib_ldflags)',
            '-Wl,--end-group'
          ],
        }],
      ],
      'dependencies': [
        '../mapboxgl.gyp:mbgl-standalone',
        '../mapboxgl.gyp:mbgl-linux',
        '../mapboxgl.gyp:copy_styles',
        '../mapboxgl.gyp:copy_certificate_bundle',
      ],
    },
  ],
}
