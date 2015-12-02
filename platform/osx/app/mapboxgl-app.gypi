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
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'INFOPLIST_FILE': '../platform/osx/app/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
      },
    },
  ]
}
