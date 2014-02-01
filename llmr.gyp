{
  'includes': [
    './common.gypi',
    './config.gypi'
  ],
  'targets': [
    {
      'target_name': 'llmr-osx',
      'product_name': 'libllmr-osx',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find include -name "*.hpp")'
      ],
      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'MACOSX_DEPLOYMENT_TARGET':'10.8',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include'
      },
      'conditions': [
        ['OS == "mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
        }],
      ],
      'include_dirs':[
          './include'
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include'
          ],
      }
    },
    {
      'target_name': 'llmr-ios',
      'product_name': 'libllmr-ios',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find include -name "*.hpp")'
      ],
      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS':'iphonesimulator iphoneos',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include'
      },
      'include_dirs':[
          './include'
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include'
          ],
      }
    }
  ]
}