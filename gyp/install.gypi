{
  'conditions': [
    ['install_prefix != ""', {
      'targets': [
        { 'target_name': 'install',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'mbgl-core',
            'mbgl-standalone',
            'mbgl-headless',
            'mbgl-<(platform)',
          ],
          'copies': [
            { 'files': [ '<(standalone_product_dir)/libmbgl.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-core.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-headless.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-<(platform).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '../include/mbgl' ], 'destination': '<(install_prefix)/include' },
          ],
          'variables': {
            'conditions': [
              ['OS == "linux"', {
                  'other_ldflags': [
                      '<@(nu_static_libs)',
                      '<@(png_static_libs)',
                      '<@(jpeg_static_libs)',
                      '<@(glfw3_static_libs)',
                      '<@(glfw3_ldflags)',
                  ]
              }, {
                  'other_ldflags': [ ]
              }]
            ],
          },
          'actions': [
              { 'action_name': 'mbgl-config',  
                'inputs': [
                    '../utils/mbgl-config/mbgl-config.template.sh',
                    '../utils/mbgl-config/build.sh',
                ],
                'outputs': [
                    '<(install_prefix)/bin/mbgl-config',
                ],
                'action': [
                    './utils/mbgl-config/build.sh',
                    '<(install_prefix)',
                    '<(platform)',
                    '<@(sqlite3_static_libs)',
                    '<@(sqlite3_ldflags)',
                    '<@(curl_static_libs)',
                    '<@(boringssl_static_libs)',
                    '<@(png_ldflags)',
                    '<@(other_ldflags)'
                ]
              }
          ]
        },
        { 'target_name': 'copy_version',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'install',
          ],
          'copies': [
            { 'files': [ '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp' ], 'destination': '<(install_prefix)/include/mbgl/util' },
          ],
        }
      ]
    }],
  ],
}
