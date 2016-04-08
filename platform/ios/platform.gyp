{
  'variables': {
    'loop_lib': 'darwin',
    'headless_lib': 'eagl',
  },
  'xcode_settings': {
     # Force all build output to the build directory. Must
     # be an absolute path or xcodebuild will ignore it.
    'SYMROOT': '<!(cd ../../build/ios-all && pwd)',
  },
  'includes': [
    '../../mbgl.gypi',
    '../../test/test.gypi',
  ],
  'target_defaults': {
    'target_conditions': [
      ['_type == "static_library"', {
        'xcode_settings': {
          'SDKROOT': 'iphoneos',
          'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
          'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CODE_SIGN_IDENTITY': 'iPhone Developer',
        },
        'configurations': {
          'Release': {
            'xcode_settings': {
              'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
            },
          },
        },
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'test',
      'type': 'executable',
      'product_name': 'ios-test',
      'product_extension': 'app',
      'mac_bundle': 1,

      'dependencies': [
        'test-lib',
        'platform-lib',
      ],

      'sources': [
        '../../test/src/main.mm',
#        '../../src/mbgl/util/premultiply.cpp',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
        'INFOPLIST_FILE': '../../test/src/app-info.plist',
        'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'COPY_PHASE_STRIP': 'NO',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
      },

      'copies': [{
        'destination': '<(PRODUCT_DIR)/$(WRAPPER_NAME)/test',
        'files': [ '../../test/fixtures' ],
      }]
    },
    {
      'target_name': 'platform-lib',
      'product_name': 'mbgl-platform-ios',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        'include',
        '../darwin/include',
        '../default',
        '../../include',
        '../../src', # TODO: eliminate
      ],

      'sources': [
        '../default/asset_file_source.cpp',
        '../default/default_file_source.cpp',
        '../default/online_file_source.cpp',
        '../default/mbgl/storage/offline.hpp',
        '../default/mbgl/storage/offline.cpp',
        '../default/mbgl/storage/offline_database.hpp',
        '../default/mbgl/storage/offline_database.cpp',
        '../default/mbgl/storage/offline_download.hpp',
        '../default/mbgl/storage/offline_download.cpp',
        '../default/sqlite3.hpp',
        '../default/sqlite3.cpp',
        '../darwin/src/http_request_nsurl.mm',
        '../darwin/src/log_nslog.mm',
        '../darwin/src/string_nsstring.mm',
        '../darwin/src/image.mm',
        '../darwin/src/nsthread.mm',
        '../darwin/src/reachability.m',
        '../darwin/src/NSException+MGLAdditions.h',
        '../darwin/src/NSString+MGLAdditions.h',
        '../darwin/src/NSString+MGLAdditions.m',
        '../darwin/src/MGLTypes.m',
        '../darwin/src/MGLStyle.mm',
        '../darwin/src/MGLGeometry_Private.h',
        '../darwin/src/MGLGeometry.mm',
        '../darwin/src/MGLShape.m',
        '../darwin/src/MGLMultiPoint_Private.h',
        '../darwin/src/MGLMultiPoint.mm',
        '../darwin/src/MGLPointAnnotation.m',
        '../darwin/src/MGLPolyline.mm',
        '../darwin/src/MGLPolygon.mm',
        '../darwin/src/MGLMapCamera.mm',
        '../darwin/src/MGLOfflinePack.mm',
        '../darwin/src/MGLOfflinePack_Private.h',
        '../darwin/src/MGLOfflineStorage.mm',
        '../darwin/src/MGLOfflineStorage_Private.h',
        '../darwin/src/MGLOfflineRegion_Private.h',
        '../darwin/src/MGLTilePyramidOfflineRegion.mm',
        '../darwin/src/MGLAccountManager_Private.h',
        '../darwin/src/MGLAccountManager.m',
        '../darwin/src/NSBundle+MGLAdditions.h',
        '../darwin/src/NSBundle+MGLAdditions.m',
        '../darwin/src/NSProcessInfo+MGLAdditions.h',
        '../darwin/src/NSProcessInfo+MGLAdditions.m',
        'src/MGLMapboxEvents.h',
        'src/MGLMapboxEvents.m',
        'src/MGLAPIClient.h',
        'src/MGLAPIClient.m',
        'src/MGLLocationManager.h',
        'src/MGLLocationManager.m',
        'src/MGLMapView.mm',
        'src/MGLUserLocation_Private.h',
        'src/MGLUserLocation.m',
        'src/MGLUserLocationAnnotationView.h',
        'src/MGLUserLocationAnnotationView.m',
        'src/MGLAnnotationImage_Private.h',
        'src/MGLAnnotationImage.m',
        'src/MGLCompactCalloutView.h',
        'src/MGLCompactCalloutView.m',
        'vendor/SMCalloutView/SMCalloutView.h',
        'vendor/SMCalloutView/SMCalloutView.m',
        'vendor/Fabric/FABAttributes.h',
        'vendor/Fabric/FABKitProtocol.h',
        'vendor/Fabric/Fabric.h',
        'vendor/Fabric/Fabric+FABKits.h',
      ],

      'variables': {
        'cflags_cc': [
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(zlib_cflags)',
          '<@(rapidjson_cflags)',
          '<@(variant_cflags)',
        ],
        'ldflags': [
          '<@(sqlite_ldflags)',
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
          '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
          '$(SDKROOT)/System/Library/Frameworks/GLKit.framework',
          '$(SDKROOT)/System/Library/Frameworks/ImageIO.framework',
          '$(SDKROOT)/System/Library/Frameworks/MobileCoreServices.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGLES.framework',
          '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
          '$(SDKROOT)/System/Library/Frameworks/Security.framework',
          '$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework',
          '$(SDKROOT)/System/Library/Frameworks/UIKit.framework',
        ],
      },

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
          'include',
          '../darwin/include',
          '../include',
        ],
        'mac_bundle_resources': [
          '<!@(find resources -type f \! -name "README" \! -name \'.*\')',
          '<!@(find ../default/resources -type f \! -name "README" \! -name \'.der\')',
        ],
      },
    },
    {
      'target_name': 'iossdk',
      'product_name': 'Mapbox',
      'type': 'shared_library',
      'mac_bundle': 1,

      'dependencies': [
        'platform-lib',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'COMBINE_HIDPI_IMAGES': 'NO', # disable combining @2x, @3x images into .tiff files
        'CURRENT_PROJECT_VERSION': '0',
        'DEFINES_MODULE': 'YES',
        'DYLIB_INSTALL_NAME_BASE': '@rpath',
        'INFOPLIST_FILE': 'framework/Info.plist',
        'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/Frameworks',
          '@loader_path/Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.sdk.ios',
        'OTHER_LDFLAGS': [ '-stdlib=libc++', '-lstdc++' ],
        'SDKROOT': 'iphoneos',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS': [
          'iphonesimulator',
          'iphoneos',
        ],
        'VERSIONING_SYSTEM': 'apple-generic',
      },

      'mac_framework_headers': [
        'framework/Mapbox.h',
        '<!@(find ../{darwin,ios}/include -type f \! -name \'.*\' \! -name Mapbox.h)',
      ],

      'sources': [
        'framework/Mapbox.m',
      ],

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'CODE_SIGN_IDENTITY': 'iPhone Developer',
            'DEAD_CODE_STRIPPING': 'YES',
            'GCC_OPTIMIZATION_LEVEL': '0',
          },
        },
        'Release': {
          'xcode_settings': {
            'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
            'CODE_SIGN_IDENTITY': 'iPhone Distribution',
            'DEAD_CODE_STRIPPING': 'YES',
            'GCC_OPTIMIZATION_LEVEL': 's',
          },
        },
      },
    },
    {
      'target_name': 'iosapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find app/img -type f)',
        'app/Storyboard.storyboard',
        'app/points.geojson',
        'app/polyline.geojson',
        'app/threestates.geojson',
        'app/Settings.bundle/',
        'app/app-info.plist',
      ],

      'dependencies': [
        'iossdk',
      ],

      'sources': [
        'app/main.m',
        'app/MBXAppDelegate.h',
        'app/MBXAppDelegate.m',
        'app/MBXCustomCalloutView.h',
        'app/MBXCustomCalloutView.m',
        'app/MBXOfflinePacksTableViewController.h',
        'app/MBXOfflinePacksTableViewController.m',
        'app/MBXViewController.h',
        'app/MBXViewController.m',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
        'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
        'INFOPLIST_FILE': 'app/app-info.plist',
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
    },
    {
      'target_name': 'ios-bench',
      'product_name': 'Bench GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '<!@(find benchmark/img -type f)',
        'benchmark/assets/glyphs',
        'benchmark/assets/sprites',
        'benchmark/assets/tiles',
      ],

      'dependencies': [
        'iossdk',
      ],

      'sources': [
        'benchmark/main.m',
        'benchmark/MBXBenchAppDelegate.h',
        'benchmark/MBXBenchAppDelegate.m',
        'benchmark/MBXBenchViewController.h',
        'benchmark/MBXBenchViewController.mm',
        'benchmark/locations.hpp',
        'benchmark/locations.cpp',
      ],

      'xcode_settings': {
        'SDKROOT': 'iphoneos',
        'SUPPORTED_PLATFORMS': 'iphoneos',
        'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
        'INFOPLIST_FILE': 'benchmark/app-info.plist',
        'TARGETED_DEVICE_FAMILY': '1,2',
        'COMBINE_HIDPI_IMAGES': 'NO', # don't merge @2x.png images into .tiff files
        'COPY_PHASE_STRIP': 'NO',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
        'LD_RUNPATH_SEARCH_PATHS': [
          '$(inherited)',
          '@executable_path/Frameworks',
        ],
        'OTHER_LDFLAGS': [
          '-framework CoreLocation',
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
  ],
}
