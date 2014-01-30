{
  'target_defaults': {
    'default_configuration': 'Release',
    'xcode_settings': {
      'CLANG_CXX_LIBRARY': 'libc++',
      'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
      'MACOSX_DEPLOYMENT_TARGET':'10.8'
    },
    'conditions': [
      ['OS == "mac"', {
        'xcode_settings': {
          'ARCHS': [ "x86_64" ],
          'SDKROOT': 'macosx',
          'SUPPORTED_PLATFORMS':'iphonesimulator iphoneos macosx',
        }
      }],
      ['OS == "ios"', {
        'xcode_settings': {
          'ARCHS': [ "$(ARCHS_STANDARD_INCLUDING_64_BIT)" ],
          'SDKROOT': 'iphoneos',
          'SUPPORTED_PLATFORMS':'iphonesimulator iphoneos macosx',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'CODE_SIGN_IDENTITY': 'iPhone Developer',
          'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
        }
      }]
    ],
    'configurations': {
      'Debug': {
        'cflags': [ '-g', '-O0' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'OTHER_CFLAGS': [ '-g', '-O0' ]
        }
      },
      'Release': {
        'cflags': [ '-O3' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'OTHER_CFLAGS': [ '-O3' ]
        }
      }
    }
  }
}