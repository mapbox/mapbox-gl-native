{
  'includes': [
    './common.gypi',
    './config.gypi',
  ],
  'targets': [
    {
      'target_name': 'shaders',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'npm_install'
      ],
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [
            '<!@(find src -name "*.glsl")'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/include/llmr/shader/shaders.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gl.cpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gles2.cpp',
          ],
          'action': ['<@(node)', 'bin/build-shaders.js', '<(SHARED_INTERMEDIATE_DIR)'],
        }
      ],
      'direct_dependent_settings': {
        'sources': [
            '<(SHARED_INTERMEDIATE_DIR)/include/llmr/shader/shaders.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gl.cpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gles2.cpp'
        ],
        'include_dirs':[
          '<(SHARED_INTERMEDIATE_DIR)/include/',
        ]
      }
    },
    {
      'target_name': 'npm_install',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'npm install',
          'inputs': [
            'bin/package.json',
          ],
          'outputs': [
            'bin/node_modules',
          ],
          'action': ['./scripts/npm_install.sh']
        }
      ],
    },
    {
      'target_name': 'build_stylesheet',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'npm_install'
      ],
      'actions': [
        {
          'action_name': 'Build Stylesheet',
          'inputs': [
            'bin/style.json',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/bin/style.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-colors.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-functions.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-layers.min.js',
          ],
          'action': ['<@(node)', 'bin/build-style.js', '<@(_inputs)', '<(SHARED_INTERMEDIATE_DIR)/bin']
        }
      ],
      'direct_dependent_settings': {
        'sources': [
            '<(SHARED_INTERMEDIATE_DIR)/bin/style.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-colors.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-functions.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-layers.min.js',
        ],
      }
    },
    {
      'target_name': 'copy_default_stylesheet',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'build_stylesheet'
      ],
      'copies': [
        {
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/bin/style.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-colors.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-functions.min.js',
            '<(SHARED_INTERMEDIATE_DIR)/bin/fuzz-layers.min.js',
          ],
          'destination': '<(PRODUCT_DIR)'
        }
      ]
    },
    {
      'target_name': 'copy_certificate_bundle',
      'type': 'none',
      'hard_dependency': 1,
      'copies': [
        {
          'files': [ 'common/ca-bundle.crt' ],
          'destination': '<(PRODUCT_DIR)'
        }
      ]
    },
    {
      'target_name': 'llmr-x86',
      'product_name': 'llmr-x86',
      'type': 'static_library',
      'hard_dependency': 1,
      'dependencies': [
          'build_stylesheet',
          'shaders',
      ],
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(test -f "config/constants_local.cpp" && echo "config/constants_local.cpp" || echo "config/constants.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':['macosx'],
        'MACOSX_DEPLOYMENT_TARGET':'10.9',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)',
          '-I<(boost_root)/include',
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include',
          ],
          'cflags': [
              '<@(png_cflags)',
              '<@(uv_cflags)',
          ],
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
                '<@(png_cflags)',
                '<@(uv_cflags)',
            ]
          },
          'conditions': [
            ['OS == "mac"', {
              'xcode_settings': {
                'OTHER_LDFLAGS': [
                    '<@(png_libraries)',
                    '<@(uv_libraries)',
                ]
              }
            }, {
              'libraries': [
                '<@(png_libraries)',
                '<@(uv_libraries)',
              ]
            }]
          ]
      }
    },
    {
      'target_name': 'llmr-ios',
      'product_name': 'llmr-ios',
      'type': 'static_library',
      'hard_dependency': 1,
      'dependencies': [
          'build_stylesheet',
          'shaders',
      ],
      'sources': [
        '<!@(find src/ \( -name "*.cpp" ! -name shaders.hpp ! -name shaders_gles2.cpp ! -name shaders_gl.cpp \))',
        '<!@(test -f "config/constants_local.cpp" && echo "config/constants_local.cpp" || echo "config/constants.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS':['iphonesimulator','iphoneos'],
        'TARGETED_DEVICE_FAMILY': '1,2',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'GCC_INPUT_FILETYPE':'sourcecode.cpp.cpp',
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)',
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include'
          ],
          'cflags': [
              '<@(png_cflags)',
              '<@(uv_cflags)',
          ],
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
                '<@(png_cflags)',
                '<@(uv_cflags)',
            ]
          },
          'conditions': [
            ['OS == "mac"', {
              'xcode_settings': {
                'OTHER_LDFLAGS': [
                    '<@(png_libraries)',
                    '<@(uv_libraries)',
                ]
              }
            }, {
              'libraries': [
                '<@(png_libraries)',
                '<@(uv_libraries)'
              ]
            }]
          ]
      }
    }
  ]
}
