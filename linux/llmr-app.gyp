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
        '../llmr.gyp:llmr-x86',
        'default_stylesheet',
        'certificate_bundle',
      ],
    },
    {
      'target_name': 'default_stylesheet',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        '../llmr.gyp:build_stylesheet'
      ],
      'copies': [
        {
          'files': [ '<(SHARED_INTERMEDIATE_DIR)/bin/style.min.js' ],
          'destination': '<(PRODUCT_DIR)'
        }
      ]
    },
    {
      'target_name': 'certificate_bundle',
      'type': 'none',
      'hard_dependency': 1,
      'copies': [
        {
          'files': [ '../common/ca-bundle.crt' ],
          'destination': '<(PRODUCT_DIR)'
        }
      ]
    }
  ],
}
