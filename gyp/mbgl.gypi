{
  'targets': [
    { 'target_name': 'mbgl',
      'product_name': 'mbgl',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
          'shaders',
      ],
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(test -f "config/constants_local.cpp" && echo "config/constants_local.cpp" || echo "config/constants.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        'bin/style.json'
      ],
      'include_dirs': [
        '../include'
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'PUBLIC_HEADERS_FOLDER_PATH': 'include',
            'OTHER_CPLUSPLUSFLAGS': [
              '<@(png_cflags)',
              '<@(uv_cflags)',
              '<@(sqlite3_cflags)',
              '<@(zlib_cflags)',
              '-I<(boost_root)/include',
            ],
            'OTHER_CFLAGS': [
              '<@(uv_cflags)',
            ],
          },
        }, {
          'cflags': [
            '<@(png_cflags)',
            '<@(uv_cflags)',
            '<@(sqlite3_cflags)',
            '<@(zlib_cflags)',
            '-I<(boost_root)/include',
          ],
        }]
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '<@(png_libraries)',
                '<@(uv_libraries)',
                '<@(sqlite3_libraries)',
                '<@(zlib_libraries)',
              ]
            }
          }, {
            'libraries': [
              '<@(png_libraries)',
              '<@(uv_libraries)',
              '<@(sqlite3_libraries)',
              '<@(zlib_libraries)',
            ]
          }]
        ]
      }
    }
  ]
}
