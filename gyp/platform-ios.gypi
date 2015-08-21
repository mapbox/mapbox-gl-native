{
  'includes': [ 'platform-darwin.gypi' ],
  'targets': [
    { 'target_name': 'platform-ios',
      'product_name': 'mbgl-platform-ios',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'platform-darwin',
      ],

      'sources': [
        '../include/mbgl/ios/Mapbox.h',
        '../platform/ios/MGLMapboxEvents.h',
        '../platform/ios/MGLMapboxEvents.m',
        '../include/mbgl/ios/MGLMapView.h',
        '../include/mbgl/ios/MGLMapView+IBAdditions.h',
        '../platform/ios/MGLMapView.mm',
        '../platform/ios/MGLFileCache.h',
        '../platform/ios/MGLFileCache.mm',
        '../include/mbgl/ios/MGLAccountManager.h',
        '../platform/ios/MGLAccountManager_Private.h',
        '../platform/ios/MGLAccountManager.m',
        '../include/mbgl/ios/MGLAnnotation.h',
        '../include/mbgl/ios/MGLUserLocation.h',
        '../platform/ios/MGLUserLocation_Private.h',
        '../platform/ios/MGLUserLocation.m',
        '../platform/ios/MGLUserLocationAnnotationView.h',
        '../platform/ios/MGLUserLocationAnnotationView.m',
        '../include/mbgl/ios/MGLTypes.h',
        '../platform/ios/MGLTypes.m',
        '../include/mbgl/ios/MGLGeometry.h',
        '../platform/ios/MGLGeometry.m',
        '../include/mbgl/ios/MGLMultiPoint.h',
        '../platform/ios/MGLMultiPoint_Private.h',
        '../platform/ios/MGLMultiPoint.mm',
        '../include/mbgl/ios/MGLOverlay.h',
        '../include/mbgl/ios/MGLPointAnnotation.h',
        '../platform/ios/MGLPointAnnotation.m',
        '../include/mbgl/ios/MGLPolyline.h',
        '../platform/ios/MGLPolyline.m',
        '../include/mbgl/ios/MGLPolygon.h',
        '../platform/ios/MGLPolygon.m',
        '../include/mbgl/ios/MGLShape.h',
        '../platform/ios/MGLShape.m',
        '../include/mbgl/ios/MGLAnnotationImage.h',
        '../platform/ios/MGLAnnotationImage.m',
        '../platform/ios/NSBundle+MGLAdditions.h',
        '../platform/ios/NSBundle+MGLAdditions.m',
        '../platform/ios/NSException+MGLAdditions.h',
        '../platform/ios/NSProcessInfo+MGLAdditions.h',
        '../platform/ios/NSProcessInfo+MGLAdditions.m',
        '../platform/ios/NSString+MGLAdditions.h',
        '../platform/ios/NSString+MGLAdditions.m',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.h',
        '../platform/ios/vendor/SMCalloutView/SMCalloutView.m',
      ],

      'variables': {
        'ldflags': [
          '-framework CoreLocation',
          '-framework CoreTelephony',
          '-framework GLKit',
          '-framework ImageIO',
          '-framework MobileCoreServices',
          '-framework QuartzCore',
          '-framework SystemConfiguration',
          '-ObjC',
        ],
      },

      'include_dirs': [
        '../include',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
      },

      'link_settings': {
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
