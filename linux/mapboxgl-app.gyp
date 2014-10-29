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
            'OTHER_LDFLAGS': [
              '<@(glfw3_ldflags)',
            ],
          }
        },

        # Non-Mac OS X
        {
          'cflags_cc': [
            '<@(glfw3_cflags)',
            '-I<(boost_root)/include',
          ],
          'ldflags': [
            '<@(glfw3_ldflags)',
            '-L<(boost_root)/lib',
            '-lboost_regex',
          ],
        }],
      ],
      'dependencies': [
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-linux',
        '../mapboxgl.gyp:copy_styles',
        '../mapboxgl.gyp:copy_certificate_bundle',
      ],
    },
  ],
}
