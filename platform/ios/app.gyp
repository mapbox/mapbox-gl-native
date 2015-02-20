{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'iosapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find ./app/img -type f)',
      ],

      'dependencies': [
        '../../mbgl.gyp:bundle_styles',
        '../../mbgl.gyp:core',
        '../../mbgl.gyp:platform-<(platform_lib)',
        '../../mbgl.gyp:http-<(http_lib)',
        '../../mbgl.gyp:asset-<(asset_lib)',
        '../../mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        'app/main.m',
        'app/MBXAppDelegate.h',
        'app/MBXAppDelegate.m',
        'app/MBXViewController.h',
        'app/MBXViewController.mm',
        '../darwin/src/settings_nsuserdefaults.mm',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
        'INFOPLIST_FILE': 'app/app-info.plist',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'COMBINE_HIDPI_IMAGES': 'NO', # don't merge @2x.png images into .tiff files
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Developer',
          },
        },
        'Release': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Distribution',
            'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
          },
        },
      },
    }
  ]
}
