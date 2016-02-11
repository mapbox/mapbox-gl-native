{
  'includes': [
    '../../../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'iossdk',
      'product_name': 'Mapbox',
      'type': 'shared_library',
      'mac_bundle': 1,
      
      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'COMBINE_HIDPI_IMAGES': 'NO', # disable combining @2x, @3x images into .tiff files
        'CURRENT_PROJECT_VERSION': '0',
        'DEFINES_MODULE': 'YES',
        'DYLIB_INSTALL_NAME_BASE': '@rpath',
        'INFOPLIST_FILE': '../platform/ios/framework/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/Frameworks',
          '@loader_path/Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.sdk.ios',
        'OTHER_LDFLAGS': [ '-stdlib=libc++', '-lstdc++' ],
        'SDKROOT': 'iphoneos',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS': [
          'iphonesimulator',
          'iphoneos',
        ],
        'VERSIONING_SYSTEM': 'apple-generic',
      },
      
      'mac_framework_headers': [
        'Mapbox.h',
        '<!@(find ../platform/{darwin,ios}/include -type f \! -name \'.*\' \! -name Mapbox.h)',
      ],
      
      'sources': [
        'Mapbox.m',
      ],
      
      'configurations': {
        'Debug': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Developer',
            'DEAD_CODE_STRIPPING': 'YES',
            'GCC_OPTIMIZATION_LEVEL': '0',
          },
        },
        'Release': {
          'xcode_settings': {
            'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
            'CODE_SIGN_IDENTITY': 'iPhone Distribution',
            'DEAD_CODE_STRIPPING': 'YES',
            'GCC_OPTIMIZATION_LEVEL': 's',
          },
        },
      },
    },
  ]
}
