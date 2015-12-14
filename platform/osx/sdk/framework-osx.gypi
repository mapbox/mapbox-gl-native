{
  'includes': [
    '../../../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'osxsdk',
      'product_name': 'Mapbox',
      'type': 'shared_library',
      'mac_bundle': 1,
      
      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
        'mbgl.gyp:cache-<(cache_lib)',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CURRENT_PROJECT_VERSION': '0',
        'DEFINES_MODULE': 'YES',
        'DYLIB_INSTALL_NAME_BASE': '@rpath',
        'INFOPLIST_FILE': '../platform/osx/sdk/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/../Frameworks',
          '@loader_path/Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
        'OTHER_LDFLAGS': [ '-stdlib=libc++', '-lstdc++' ],
        'SDKROOT': 'macosx',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS':'macosx',
        'VERSIONING_SYSTEM': 'apple-generic',
      },
      
      'mac_framework_headers': [
        './Mapbox.h',
        '<!@(find ../include/mbgl/{darwin,osx} -type f \! -name \'.*\')',
      ],
      
      'sources': [
        './Mapbox.m',
      ],
      
      'configurations': {
        'Debug': {
          'xcode_settings': {
            'GCC_OPTIMIZATION_LEVEL': '0',
          },
        },
        'Release': {
          'xcode_settings': {
            'GCC_OPTIMIZATION_LEVEL': 's',
          },
        },
      },
      
      'direct_dependent_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
        ],
      },
    },
  ]
}
