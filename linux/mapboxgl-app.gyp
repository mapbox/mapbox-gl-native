{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'linuxapp',
      'product_name': 'mapbox-gl',
      'type': 'executable',

      'dependencies': [
        '../mbgl.gyp:<(core_library)',
        '../mbgl.gyp:<(platform_library)',
        '../mbgl.gyp:<(storage_library)',
        '../mbgl.gyp:bundle_styles',
        '../mbgl.gyp:copy_certificate_bundle',
      ],

      'sources': [
        'main.cpp',
        '../platform/default/settings_json.cpp',
        '../platform/default/glfw_view.cpp',
        '../platform/default/log_stderr.cpp',
      ],

      'variables' : {
        'cflags_cc': [
          '<@(glfw3_cflags)',
        ],
        'ldflags': [
          '<@(glfw3_ldflags)',
        ],
        'libraries': [
          '<@(glfw3_static_libs)',
        ],
      },

      'libraries': [ '<@(libraries)' ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'ldflags': [ '<@(ldflags)' ],
        }]
      ],

      'copies': [{
        'files': [ '../styles/styles' ],
        'destination': '<(PRODUCT_DIR)'
      }],
    },
  ],
}
