{
  'includes': [
    '../../../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'osxtest',
      'product_name': 'osxtest',
      'type': 'loadable_module',
      'mac_xctest_bundle': 1,
      
      'dependencies': [
        'osxsdk',
      ],

      'variables': {
        'cflags_cc': [
        ],
        'ldflags': [
          '-stdlib=libc++',
          '-lstdc++',
        ],
      },

      'xcode_settings': {
        'CLANG_ENABLE_MODULES': 'YES',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'ENABLE_STRICT_OBJC_MSGSEND': 'YES',
        'GCC_DYNAMIC_NO_PIC': 'NO',
        'GCC_NO_COMMON_BLOCKS': 'YES',
        'INFOPLIST_FILE': '../platform/osx/test/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '${inherited}',
          '@executable_path/../Frameworks',
          '@loader_path/../Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.osxtest',
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        'SDKROOT': 'macosx',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS':'macosx',
      },
      
      'sources': [
        './MGLGeometryTests.mm',
        './MGLStyleTests.mm',
      ],

      'include_dirs': [
        '../../../platform/osx/include',
        '../../../platform/darwin/include',
        '../../../include',
        '../../../src',
      ],
      
      'configurations': {
        'Debug': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'NO',
            'DEBUG_INFORMATION_FORMAT': 'dwarf',
            'ENABLE_TESTABILITY': 'YES',
            'GCC_OPTIMIZATION_LEVEL': '0',
            'GCC_PREPROCESSOR_DEFINITIONS': [
              'DEBUG=1',
              '${inherited}',
            ],
            'ONLY_ACTIVE_ARCH': 'YES',
          },
        },
        'Release': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'YES',
            'DEBUG_INFORMATION_FORMAT': 'dwarf-with-dsym',
            'ENABLE_NS_ASSERTIONS': 'NO',
          },
        },
      },
    },
  ]
}
