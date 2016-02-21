{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'linuxapp',
      'product_name': 'mapbox-gl',
      'type': 'executable',

      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
        'mbgl.gyp:copy_certificate_bundle',
      ],

      'sources': [
        'main.cpp',
        '../default/settings_json.cpp',
        '../default/glfw_view.hpp',
        '../default/glfw_view.cpp',
        '../default/log_stderr.cpp',
      ],

      'variables' : {
        'cflags_cc': [
          '<@(opengl_cflags)',
          '<@(boost_cflags)',
          '<@(glfw_cflags)',
        ],
        'ldflags': [
          '<@(glfw_ldflags)',
        ],
        'libraries': [
          '<@(glfw_static_libs)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'SDKROOT': 'macosx',
            'SUPPORTED_PLATFORMS':'macosx',
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'MACOSX_DEPLOYMENT_TARGET': '10.10',
          },
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
    },
  ],
}
