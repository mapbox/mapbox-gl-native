{
  'includes': [
    '../../../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'Credits.rtf',
        'Icon.icns',
        'MainMenu.xib',
      ],

      'dependencies': [
        'osxsdk',
      ],

      'sources': [
        './AppDelegate.h',
        './AppDelegate.m',
        './DroppedPinAnnotation.h',
        './DroppedPinAnnotation.m',
        './LocationCoordinate2DTransformer.h',
        './LocationCoordinate2DTransformer.m',
        './TimeIntervalTransformer.h',
        './TimeIntervalTransformer.m',
        './NSValue+Additions.h',
        './NSValue+Additions.m',
        './main.m',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'INFOPLIST_FILE': '../platform/osx/app/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/../Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS': 'macosx',
      },
      
      'configurations': {
        'Debug': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'NO',
          },
        },
        'Release': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'YES',
          },
        },
      },
      
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)/$(FRAMEWORKS_FOLDER_PATH)',
          'files': [
            '<(PRODUCT_DIR)/Mapbox.framework',
          ],
          'xcode_code_sign': 1,
        }
      ],
    },
  ]
}
