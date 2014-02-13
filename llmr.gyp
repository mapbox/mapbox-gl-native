{
  'includes': [
    './common.gypi',
    './config.gypi'
  ],
  'targets': [
    {
      'target_name': 'llmr-osx',
      'product_name': 'llmr-osx',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find include -name "*.hpp")',
        '<!@(find src -name "*.glsl")'
      ],
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [],
          'outputs': [],
          'action': ['bin/build-shaders.js'],
        },
      ],
      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':['macosx'],
        'MACOSX_DEPLOYMENT_TARGET':'10.8',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'OTHER_CPLUSPLUSFLAGS':[
            '<@(png_cflags)'
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)'
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
    },
    {
      'target_name': 'llmr-ios',
      'product_name': 'llmr-ios',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find include -name "*.hpp")',
        '<!@(find src -name "*.glsl")'
      ],
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [],
          'outputs': [],
          'action': ['bin/build-shaders.js'],
        },
      ],
      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS':['iphonesimulator','iphoneos',],
        'ARCHS': [ "armv7", "armv7s", "arm64", "i386" ],
        'TARGETED_DEVICE_FAMILY': '1,2',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
        'GCC_INPUT_FILETYPE':'sourcecode.cpp.cpp',
        'OTHER_CPLUSPLUSFLAGS':[
            '<@(png_cflags)'
        ]
      },
      'include_dirs':[
          './include'
      ],
      'cflags': [
          '<@(png_cflags)'
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