{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'tvosapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find ../tvos/app/img -type f)',
        './points.geojson',
        './polyline.geojson',
        './threestates.geojson',
        './Settings.bundle/',
        './app-info.plist'
      ],

      'dependencies': [
        '../mbgl.gyp:bundle_styles',
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        './main.m',
        './MBXAppDelegate.h',
        './MBXAppDelegate.m',
        './MBXViewController.h',
        './MBXViewController.mm',
      ],

      'xcode_settings': {
        'SDKROOT': 'appletvos',
        'SUPPORTED_PLATFORMS': 'appletvsimulator appletvos',
        'APPLE_DEPLOYMENT_TARGET': '9.0',
        'INFOPLIST_FILE': '../tvos/app/app-info.plist',
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
