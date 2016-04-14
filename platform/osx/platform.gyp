{
  'variables': {
    'loop_lib': 'darwin',
    'headless_lib': 'cgl',
  },
  'xcode_settings': {
     # Force all build output to the build directory. Must
     # be an absolute path or xcodebuild will ignore it.
    'SYMROOT': '<!(cd ../../build/osx-x86_64 && pwd)',
  },
  'includes': [
    '../../mbgl.gypi',
    '../../test/test.gypi',
    '../../bin/glfw.gypi',
    '../../bin/render.gypi',
    '../../bin/offline.gypi',
  ],
  'targets': [
    {
      'target_name': 'test',
      'type': 'executable',

      'dependencies': [
        'test-lib',
        'platform-lib',
      ],

      'sources': [
        '../../test/src/main.cpp',
      ],
    },
    {
      'target_name': 'platform-lib',
      'product_name': 'mbgl-platform-osx',
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
        '../darwin/src/http_file_source.mm',
        '../darwin/src/log_nslog.mm',
        '../darwin/src/string_nsstring.mm',
        '../darwin/src/image.mm',
        '../darwin/src/nsthread.mm',
        '../darwin/src/reachability.m',
      ],

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(zlib_cflags)',
          '<@(rapidjson_cflags)',
          '<@(variant_cflags)',
        ],
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CLANG_ENABLE_MODULES': 'YES',
      },

      'link_settings': {
        'libraries': [
          '<@(sqlite_static_libs)',
          '<@(zlib_static_libs)',
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
          '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
          '$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(zlib_ldflags)' ],
        },
      },
    },
    {
      'target_name': 'osxsdk',
      'product_name': 'Mapbox',
      'type': 'shared_library',
      'mac_bundle': 1,

      'dependencies': [
        'platform-lib',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'CURRENT_PROJECT_VERSION': '0',
        'DEFINES_MODULE': 'YES',
        'DYLIB_INSTALL_NAME_BASE': '@rpath',
        'INFOPLIST_FILE': 'sdk/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '${inherited}',
          '@executable_path/../Frameworks',
          '@loader_path/Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(boost_cflags)',
          '<@(variant_cflags)',
        ],
        'OTHER_LDFLAGS': [ '-stdlib=libc++', '-lstdc++' ],
        'SDKROOT': 'macosx',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS':'macosx',
        'VERSIONING_SYSTEM': 'apple-generic',
      },

      'mac_framework_headers': [
        'sdk/Mapbox.h',
        '<!@(find ../{darwin,osx}/include -type f \! -name \'.*\')',
      ],

      'mac_bundle_resources': [
        '<!@(find src/resources -type f \! -name \'.*\')',
      ],

      'include_dirs': [
        'include',
        '../darwin/include',
        '../../include',
      ],

      'sources': [
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
        'src/MGLMapView_Private.h',
        'src/MGLMapView.mm',
        'src/MGLMapView+IBAdditions.m',
        'src/MGLOpenGLLayer.h',
        'src/MGLOpenGLLayer.mm',
        'src/MGLCompassCell.h',
        'src/MGLCompassCell.m',
        'src/MGLAttributionButton.h',
        'src/MGLAttributionButton.m',
        'src/MGLAnnotationImage.m',
      ],

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'GCC_OPTIMIZATION_LEVEL': '0',
          },
        },
        'Release': {
          'xcode_settings': {
            'GCC_OPTIMIZATION_LEVEL': 's',
          },
        },
      },

      'direct_dependent_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
        ],
      },
    },
    {
      'target_name': 'osxtest',
      'product_name': 'osxtest',
      'type': 'loadable_module',
      'mac_xctest_bundle': 1,

      'dependencies': [
        'osxsdk',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_MODULES': 'YES',
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'ENABLE_STRICT_OBJC_MSGSEND': 'YES',
        'GCC_DYNAMIC_NO_PIC': 'NO',
        'GCC_NO_COMMON_BLOCKS': 'YES',
        'INFOPLIST_FILE': 'test/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '${inherited}',
          '@executable_path/../Frameworks',
          '@loader_path/../Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.osxtest',
        'SDKROOT': 'macosx',
        'SKIP_INSTALL': 'YES',
        'SUPPORTED_PLATFORMS':'macosx',
      },

      'sources': [
        '../darwin/test/MGLGeometryTests.mm',
        '../darwin/test/MGLOfflinePackTests.m',
        '../darwin/test/MGLOfflineRegionTests.m',
        '../darwin/test/MGLOfflineStorageTests.m',
        '../darwin/test/MGLStyleTests.mm',
      ],

      'include_dirs': [
        'include',
        '../darwin/include',
        '../../include',
      ],

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'NO',
            'DEBUG_INFORMATION_FORMAT': 'dwarf',
            'ENABLE_TESTABILITY': 'YES',
            'GCC_OPTIMIZATION_LEVEL': '0',
            'GCC_PREPROCESSOR_DEFINITIONS': [
              'DEBUG=1',
              '${inherited}',
            ],
            'ONLY_ACTIVE_ARCH': 'YES',
          },
        },
        'Release': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'YES',
            'DEBUG_INFORMATION_FORMAT': 'dwarf-with-dsym',
            'ENABLE_NS_ASSERTIONS': 'NO',
          },
        },
      },
    },
    {
      'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'app/Credits.rtf',
        'app/Icon.icns',
        'app/MainMenu.xib',
        'app/MapDocument.xib',
      ],

      'dependencies': [
        'osxsdk',
      ],

      'sources': [
        'app/AppDelegate.h',
        'app/AppDelegate.m',
        'app/DroppedPinAnnotation.h',
        'app/DroppedPinAnnotation.m',
        'app/LocationCoordinate2DTransformer.h',
        'app/LocationCoordinate2DTransformer.m',
        'app/MapDocument.h',
        'app/MapDocument.m',
        'app/OfflinePackNameValueTransformer.h',
        'app/OfflinePackNameValueTransformer.m',
        'app/TimeIntervalTransformer.h',
        'app/TimeIntervalTransformer.m',
        'app/NSValue+Additions.h',
        'app/NSValue+Additions.m',
        'app/main.m',
      ],

      'xcode_settings': {
        'CLANG_ENABLE_OBJC_ARC': 'YES',
        'INFOPLIST_FILE': 'app/Info.plist',
        'LD_RUNPATH_SEARCH_PATHS': [
          '\${inherited}',
          '@executable_path/../Frameworks',
        ],
        'PRODUCT_BUNDLE_IDENTIFIER': 'com.mapbox.MapboxGL',
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS': 'macosx',
      },

      'configurations': {
        'Debug': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'NO',
          },
        },
        'Release': {
          'xcode_settings': {
            'COPY_PHASE_STRIP': 'YES',
          },
        },
      },

      'copies': [
        {
          'destination': '<(PRODUCT_DIR)/${FRAMEWORKS_FOLDER_PATH}',
          'files': [
            '<(PRODUCT_DIR)/Mapbox.framework',
          ],
          'xcode_code_sign': 1,
        }
      ],
    },
  ],
}
