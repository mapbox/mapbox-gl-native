{
  'conditions': [
    ['install_prefix != ""', {
      'targets': [
        { 'target_name': 'install',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'core',
            'platform-<(platform_lib)',
            'http-<(http_lib)',
            'asset-<(asset_lib)',
            'cache-<(cache_lib)',
            'headless-<(headless_lib)',
            'standalone',
          ],
          'copies': [
            { 'files': [ '<(standalone_product_dir)/libmbgl.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-core.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/lib<(headless_lib).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/lib<(platform_lib).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/lib<(http_lib).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/lib<(asset_lib).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/lib<(cache_lib).a' ], 'destination': '<(install_prefix)/lib' },
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
                    '<@(sqlite3_static_libs)',
                    '<@(sqlite3_ldflags)',
                    '<@(curl_ldflags)',
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
