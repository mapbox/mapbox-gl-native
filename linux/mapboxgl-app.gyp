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
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(glfw3_cflags)',
        ],
      },
      'cflags_cc': [
        '<@(glfw3_cflags)',
        '-I<(boost_root)/include',
      ],
      'libraries': [
        '<@(png_ldflags)',
        '<@(sqlite3_ldflags)',
        '<@(glfw3_static_libs)',
        '<@(glfw3_ldflags)',
        '<@(curl_ldflags)',
        '<@(zlib_ldflags)',
      ],
      'conditions': [
        ['OS == "linux"', {
            'libraries':[ '-L<(boost_root)/lib','-lboost_regex' ]
        }]
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
