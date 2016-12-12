{
  'variables': {
    'mason_platform': '<!(echo $MASON_PLATFORM)',
  },
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
        'cflags_cc': [ '-g', '-O0', '-std=c++14', '-fno-omit-frame-pointer','-fwrapv', '-fstack-protector-all', '-fno-common' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'NO',
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'NO',
          'DEAD_CODE_STRIPPING': 'NO',
          'OTHER_CPLUSPLUSFLAGS': [ '-std=c++14', '-fno-omit-frame-pointer','-fwrapv', '-fstack-protector-all', '-fno-common'],
        }
      },
      'Release': {
        'cflags_cc': [ '-O3', '-std=c++14', '-fvisibility=hidden' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3',
          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
          'DEAD_CODE_STRIPPING': 'NO',
          'OTHER_CPLUSPLUSFLAGS': [ '-std=c++14' ],
        }
      },
    },
    'target_conditions': [
      ['OS == "mac"', {
        'conditions': [
          ['mason_platform == "ios"', {
            'xcode_settings': {
              'SDKROOT': 'iphoneos',
              'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
              'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
              'TARGETED_DEVICE_FAMILY': '1,2',
              'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
            },
          }, {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.9',
            },
          }],
        ],
      }],
    ],
  },
}
