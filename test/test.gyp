{
  'includes': [
    '../gyp/common.gypi',
    '../gyp/version.gypi',
    '../gyp/mbgl-platform.gypi',
  ],
  'targets': [
    { 'target_name': 'test_base',
      'type': 'none',
      'direct_dependent_settings': {
        'include_dirs': [ '../include', '../src' ],
      },
      'link_settings': {
        'libraries': [
          '<@(uv_static_libs)',
          '<@(glfw3_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(zlib_static_libs)',
        ],
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '<@(uv_ldflags)',
            '<@(glfw3_ldflags)',
            '<@(sqlite3_ldflags)',
            '<@(zlib_ldflags)',
          ],
          'OTHER_CFLAGS': [ '<@(uv_cflags)' ],
          'OTHER_CPLUSPLUSFLAGS': [ '<@(uv_cflags)' ],
        },
        'cflags': [ '<@(uv_cflags)' ],
        'cxxflags': [ '<@(uv_cflags)' ],
      },
    },

    # Build all targets
    { 'target_name': 'test',
      'type': 'executable',
      'dependencies': [
        'test_base',
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../mapboxgl.gyp:mbgl-headless',
        '../deps/gtest/gtest.gyp:gtest'
      ],
      'sources': [
        'main.cpp',

        'headless/headless.cpp',

        'miscellaneous/clip_ids.cpp',
        'miscellaneous/comparisons.cpp',
        'miscellaneous/enums.cpp',
        'miscellaneous/functions.cpp',
        'miscellaneous/merge_lines.cpp',
        'miscellaneous/rotation_range.cpp',
        'miscellaneous/style_parser.cpp',
        'miscellaneous/text_conversions.cpp',
        'miscellaneous/tile.cpp',
        'miscellaneous/variant.cpp',

        'storage/cache_response.cpp',
        'storage/cache_revalidate.cpp',
        'storage/directory_reading.cpp',
        'storage/file_reading.cpp',
        'storage/http_cancel.cpp',
        'storage/http_coalescing.cpp',
        'storage/http_error.cpp',
        'storage/http_header_parsing.cpp',
        'storage/http_load.cpp',
        'storage/http_noloop.cpp',
        'storage/http_other_loop.cpp',
        'storage/http_reading.cpp',
      ],
    },
  ]
}
