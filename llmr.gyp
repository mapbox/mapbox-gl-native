{
  'includes': [
    './common.gypi',
    './config.gypi',
  ],
  'targets': [
    {
      'target_name': 'shaders',
      'type': 'none',
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [
            '<!@(find src -name "*.glsl")'
          ],
          'outputs': [
            'include/llmr/shader/shaders.hpp',
            'src/shader/shaders_gl.cpp',
            'src/shader/shaders_gles2.cpp',
          ],
          'action': ['bin/build-shaders.js'],
        }
      ]
    },
    {
      'target_name': 'build_stylesheet',
      'type': 'none',
      'actions': [
        {
          'action_name': 'Build Stylesheet',
          'inputs': [
            'bin/style.js'
          ],
          'outputs': [
            'include/llmr/style/resources.hpp',
            'src/style/resources.cpp'
          ],
          'action': ['bin/build-style.js']
        }
      ],
    },
    {
      'target_name': 'llmr-x86',
      'product_name': 'llmr-x86',
      'type': 'static_library',
      'dependencies': [
          'build_stylesheet',
          'shaders',
      ],
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.js'
      ],
      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':['macosx'],
        'MACOSX_DEPLOYMENT_TARGET':'10.9',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(png_cflags)',
          '-I<(boost_root)/include',
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)'
          '-I<(boost_root)/include',
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include'
          ],
          'cflags': [
              '<@(png_cflags)',
          ],
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
                '<@(png_cflags)',
            ]
          },
          'conditions': [
            ['OS == "mac"', {
              'xcode_settings': {
                'OTHER_LDFLAGS': [
                    '<@(png_libraries)'
                ]
              }
            }, {
              'libraries': [
                '<@(png_libraries)'
              ]
            }]
          ]
      }
    },
    {
      'target_name': 'llmr-ios',
      'product_name': 'llmr-ios',
      'type': 'static_library',
      'dependencies': [
          'build_stylesheet',
          'shaders',
      ],
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.js'
      ],
      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS':['iphonesimulator','iphoneos',],
        'ARCHS': [ "armv7", "armv7s", "arm64", "i386" ],
        'TARGETED_DEVICE_FAMILY': '1,2',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'GCC_INPUT_FILETYPE':'sourcecode.cpp.cpp',
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(png_cflags)',
          '-I<(boost_root)/include',
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)'
          '-I<(boost_root)/include',
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include'
          ],
          'cflags': [
              '<@(png_cflags)'
          ],
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
                '<@(png_cflags)'
            ]
          },
          'conditions': [
            ['OS == "mac"', {
              'xcode_settings': {
                'OTHER_LDFLAGS': [
                    '<@(png_libraries)'
                ]
              }
            }, {
              'libraries': [
                '<@(png_libraries)'
              ]
            }]
          ]
      }
    }
  ]
}