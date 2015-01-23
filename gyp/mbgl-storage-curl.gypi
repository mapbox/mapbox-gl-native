{
  'targets': [
    { 'target_name': 'mbgl-storage-curl',
      'product_name': 'mbgl-storage-curl',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/default_file_source.cpp',
        '../platform/default/sqlite_cache.cpp',
        '../platform/default/http_request_curl.cpp',
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

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],
      ],

      'link_settings': {
        'libraries': [ '<@(libraries)' ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'ldflags': [ '<@(ldflags)' ],
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [ '../include' ],
      },
    },
  ],
}
