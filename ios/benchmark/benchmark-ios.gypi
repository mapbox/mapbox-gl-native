{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'ios-bench',
      'product_name': 'Bench GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find ../ios/benchmark/img -type f)',
        'assets/glyphs',
        'assets/sprites',
        'assets/styles',
        'assets/tiles',
      ],

      'dependencies': [
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        './main.m',
        './MBXBenchAppDelegate.h',
        './MBXBenchAppDelegate.m',
        './MBXBenchViewController.h',
        './MBXBenchViewController.mm',
        './locations.hpp',
        './locations.cpp',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphoneos',
        'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
        'INFOPLIST_FILE': '../ios/benchmark/app-info.plist',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'COMBINE_HIDPI_IMAGES': 'NO', # don't merge @2x.png images into .tiff files
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
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
            'ARCHS': [ "arm64" ],
          },
        },
      },
    }
  ]
}
