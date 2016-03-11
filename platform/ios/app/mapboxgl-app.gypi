{
  'includes': [
    '../../../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'iosapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find ../platform/ios/app/img -type f)',
        './Storyboard.storyboard',
        './points.geojson',
        './polyline.geojson',
        './threestates.geojson',
        './Settings.bundle/',
        './app-info.plist',
      ],

      'dependencies': [
        'iossdk',
      ],

      'sources': [
        'main.m',
        'MBXAppDelegate.h',
        'MBXAppDelegate.m',
        'MBXCustomCalloutView.h',
        'MBXCustomCalloutView.m',
        'MBXOfflinePacksTableViewController.h',
        'MBXOfflinePacksTableViewController.m',
        'MBXViewController.h',
        'MBXViewController.mm',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
        'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
        'INFOPLIST_FILE': '../platform/ios/app/app-info.plist',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'COMBINE_HIDPI_IMAGES': 'NO', # disable combining @2x, @3x images into .tiff files
        'COPY_PHASE_STRIP': 'NO',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/Frameworks',
        ],
      },

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Developer',
            'COPY_PHASE_STRIP': 'NO',
          },
        },
        'Release': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Distribution',
            'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
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
        },
      ],
    }
  ]
}
