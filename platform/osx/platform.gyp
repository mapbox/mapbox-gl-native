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
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [ '<@(zlib_ldflags)' ],
        },
      },
    },
  ],
}
