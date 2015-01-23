{
  'targets': [
    { 'target_name': 'mbgl-storage-cocoa',
      'product_name': 'mbgl-storage-cocoa',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/default_file_source.cpp',
        '../platform/default/sqlite_cache.cpp',
        '../platform/darwin/http_request_cocoa.mm',
        '../platform/default/asset_request_libuv.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
        '../platform/default/compression.hpp',
        '../platform/default/compression.cpp',
      ],

      'include_dirs': [
        '../include',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(sqlite3_cflags)',
          '-I<(boost_root)/include',
        ],
        'ldflags': [
          '-framework Foundation', # For NSURLRequest
          '<@(uv_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(zlib_ldflags)',
        ],
        'libraries': [
          '<@(uv_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(zlib_static_libs)',
        ],
      },

      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'CLANG_ENABLE_OBJC_ARC': 'NO',
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
      },
    },
  ],
}
