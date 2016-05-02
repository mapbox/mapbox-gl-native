{
  'targets': [
    {
      'target_name': 'glfw-app',
      'product_name': 'mapbox-glfw',
      'type': 'executable',

      'dependencies': [
        'platform-lib',
        'copy_certificate_bundle',
      ],

      'include_dirs': [
        '../platform/default',
        '../include',
        '../src',
      ],

      'sources': [
        'glfw.cpp',
        '../platform/default/settings_json.cpp',
        '../platform/default/glfw_view.hpp',
        '../platform/default/glfw_view.cpp',
        '../platform/default/log_stderr.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '<@(glfw_cflags)',
          '<@(variant_cflags)',
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
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],
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
