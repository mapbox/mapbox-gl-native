{
  'targets': [
    { 'target_name': 'cache-sqlite',
      'product_name': 'mbgl-cache-sqlite',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '../platform/default/sqlite_cache.cpp',
        '../platform/default/sqlite3.hpp',
        '../platform/default/sqlite3.cpp',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(sqlite3_cflags)',
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
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [ '../include' ],
      },
    },
  ],
}
