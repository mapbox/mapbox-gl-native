{
  'targets': [
    { 'target_name': 'platform-osx',
      'product_name': 'mbgl-platform-osx',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'version',
      ],

      'sources': [
        '../platform/default/async_task.cpp',
        '../platform/default/run_loop.cpp',
        '../platform/default/timer.cpp',
        '../platform/default/default_file_source.cpp',
        '../platform/default/online_file_source.cpp',
        '../platform/default/sqlite_cache.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
        '../platform/darwin/log_nslog.mm',
        '../platform/darwin/string_nsstring.mm',
        '../platform/darwin/application_root.mm',
        '../platform/darwin/image.mm',
        '../platform/darwin/nsthread.mm',
        '../platform/darwin/reachability.m',
        '../platform/darwin/NSException+MGLAdditions.h',
        '../platform/darwin/NSString+MGLAdditions.h',
        '../platform/darwin/NSString+MGLAdditions.m',
        '../platform/darwin/MGLTypes.m',
        '../platform/darwin/MGLStyle.mm',
        '../platform/darwin/MGLGeometry_Private.h',
        '../platform/darwin/MGLGeometry.mm',
        '../platform/darwin/MGLShape.m',
        '../platform/darwin/MGLMultiPoint_Private.h',
        '../platform/darwin/MGLMultiPoint.mm',
        '../platform/darwin/MGLPointAnnotation.m',
        '../platform/darwin/MGLPolyline.mm',
        '../platform/darwin/MGLPolygon.mm',
        '../platform/darwin/MGLMapCamera.mm',
        '../platform/osx/src/MGLAccountManager_Private.h',
        '../platform/osx/src/MGLAccountManager.m',
        '../platform/osx/src/MGLMapView_Private.h',
        '../platform/osx/src/MGLMapView.mm',
        '../platform/osx/src/MGLMapView+IBAdditions.m',
        '../platform/osx/src/MGLOpenGLLayer.h',
        '../platform/osx/src/MGLOpenGLLayer.mm',
        '../platform/osx/src/MGLCompassCell.h',
        '../platform/osx/src/MGLCompassCell.m',
        '../platform/osx/src/MGLAttributionButton.h',
        '../platform/osx/src/MGLAttributionButton.m',
        '../platform/osx/src/MGLAnnotationImage.m',
        '../platform/osx/src/NSBundle+MGLAdditions.h',
        '../platform/osx/src/NSBundle+MGLAdditions.m',
        '../platform/osx/src/NSProcessInfo+MGLAdditions.h',
        '../platform/osx/src/NSProcessInfo+MGLAdditions.m',
      ],

      'variables': {
        'cflags_cc': [
          '<@(libuv_cflags)',
          '<@(boost_cflags)',
          '<@(variant_cflags)',
          '<@(sqlite_cflags)',
          '<@(zlib_cflags)',
        ],
        'ldflags': [
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
          '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
          '$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework',
        ],
      },

      'include_dirs': [
        '../include/mbgl/osx',
        '../include/mbgl/darwin',
        '../include',
        '../src',
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
          '../include/mbgl/osx',
          '../include/mbgl/darwin',
          '../include',
        ],
        'mac_bundle_resources': [
          '<!@(find ../platform/osx/src/resources -type f \! -name \'.*\')',
        ],
      },
    },
  ],
}
