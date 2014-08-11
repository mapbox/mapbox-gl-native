{
  'includes': [
    '../common.gypi',
    '../config.gypi',
  ],
  'targets': [
    {
      'target_name': 'linuxapp',
      'product_name': 'mapbox-gl',
      'type': 'executable',
      'sources': [
        './main.cpp',
        '../common/settings_json.cpp',
        '../common/settings_json.hpp',
        '../common/glfw_view.hpp',
        '../common/glfw_view.cpp',
        '../common/curl_request.cpp',
        '../common/stderr_log.hpp',
        '../common/stderr_log.cpp',
      ],

      'conditions': [
        ['OS == "mac"',

        # Mac OS X
        {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
              '<@(glfw3_cflags)',
              '<@(curl_cflags)',
            ],
            'OTHER_LDFLAGS': [
              '<@(glfw3_libraries)',
              '<@(curl_libraries)',
            ],
          }
        },

        # Non-Mac OS X
        {
          'cflags': [
            '<@(glfw3_cflags)',
            '<@(curl_cflags)',
          ],
          'link_settings': {
            'libraries': [
              '<@(glfw3_libraries)',
              '<@(curl_libraries)',
              '-lboost_regex'
            ],
          },
        }],
      ],
      'dependencies': [
        '../mapboxgl.gyp:mapboxgl',
        '../mapboxgl.gyp:copy_styles',
        '../mapboxgl.gyp:copy_certificate_bundle',
      ],
    },
  ],
}
