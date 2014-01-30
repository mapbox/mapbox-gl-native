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
      'mac_framework_headers': [
        '<!@(find include -name "*.hpp")'
      ],
      'xcode_settings': {
        'ARCHS': [ "x86_64" ],
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
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
          './include',
          '<(boost_root)/include'
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include',
              '<(boost_root)/include'
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
      'mac_framework_headers': [
        '<!@(find include -name "*.hpp")'
      ],
      'xcode_settings': {
        'ARCHS': [ "$(ARCHS_STANDARD_INCLUDING_64_BIT)" ],
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS':'iphonesimulator iphoneos',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
        'PUBLIC_HEADERS_FOLDER_PATH': 'include'
      },
      'include_dirs':[
          './include',
          '<(boost_root)/include'
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include',
              '<(boost_root)/include'
          ],
      }
    }
  ]
}