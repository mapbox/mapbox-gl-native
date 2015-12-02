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
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'OTHER_LDFLAGS': [ '-stdlib=libc++', '-lstdc++' ],
        'INSTALL_PATH': '@executable_path/../Frameworks',
        'INFOPLIST_FILE': '../platform/osx/sdk/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
      },
      
      'mac_framework_headers': [
        './Mapbox.h',
        '<!@(find ../include/mbgl/{darwin,osx} -type f \! -name \'.*\')',
      ],
      
      'sources': [
        './Mapbox.h',
        './Mapbox.m',
      ],
      
      'direct_dependent_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
        ],
      },
    },
  ]
}
