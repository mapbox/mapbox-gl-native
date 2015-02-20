{
  'targets': [
    { 'target_name': 'platform-ios',
      'product_name': 'mbgl-platform-ios',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../darwin/src/log_nslog.mm',
        '../darwin/src/string_nsstring.mm',
        '../darwin/src/application_root.mm',
        '../darwin/src/asset_root.mm',
        '../darwin/src/image.mm',
        '../darwin/src/reachability.m',
        './include/MGLMapView.h',
        './include/MGLStyleFunctionValue.h',
        './include/MGLTypes.h',
        './include/NSArray+MGLAdditions.h',
        './include/NSDictionary+MGLAdditions.h',
        './include/UIColor+MGLAdditions.h',
        './src/MGLMapView.mm',
        './src/MGLStyleFunctionValue.m',
        './src/MGLTypes.m',
        './src/NSArray+MGLAdditions.m',
        './src/NSDictionary+MGLAdditions.m',
        './src/UIColor+MGLAdditions.m',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(boost_cflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
        ],
        'ldflags': [
          '-framework CoreGraphics',
          '-framework CoreLocation',
          '-framework ImageIO',
          '-framework GLKit',
          '-framework MobileCoreServices',
          '-framework OpenGLES',
          '-framework SystemConfiguration',
          '-framework UIKit',
        ],
      },

      'include_dirs': [
        '../../include',
        '../darwin/include',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'YES',
      },

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        },
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../../include',
        ],
        'mac_bundle_resources': [
          '<!@(find ./platform/ios/resources -type f)',
        ],
      },
    },
  ],
}
