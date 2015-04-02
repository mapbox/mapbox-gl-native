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
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/asset_root.mm',
        '../platform/darwin/image.mm',
        '../platform/darwin/reachability.m',
        '../include/mbgl/ios/MapboxGL.h',
        '../include/mbgl/ios/MGLMapboxEvents.h',
        '../platform/ios/MGLMapboxEvents.m',
        '../include/mbgl/ios/MGLMapView.h',
        '../platform/ios/MGLMapView.mm',
        '../include/mbgl/ios/MGLAnnotation.h',
        '../include/mbgl/ios/MGLUserLocation.h',
        '../platform/ios/MGLUserLocation_Private.h',
        '../platform/ios/MGLUserLocation.m',
        '../platform/ios/MGLUserLocationAnnotationView.h',
        '../platform/ios/MGLUserLocationAnnotationView.m',
        '../include/mbgl/ios/MGLMetricsLocationManager.h',
        '../platform/ios/MGLMetricsLocationManager.m',
        '../include/mbgl/ios/MGLTypes.h',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.h',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.m',
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
          '-framework CoreLocation',
          '-framework CoreTelephony',
          '-framework GLKit',
          '-framework ImageIO',
          '-framework MobileCoreServices',
          '-framework QuartzCore',
          '-framework SystemConfiguration',
        ],
      },

      'include_dirs': [
        '../include',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
      },

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        },
      },

      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'mac_bundle_resources': [
          '<!@(find ./platform/ios/resources -type f)',
        ],
      },
    },
  ],
}
