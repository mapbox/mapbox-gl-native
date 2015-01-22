{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'test',
      'type': 'executable',
      'include_dirs': [ '../include', '../src' ],
      'dependencies': [
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../mapboxgl.gyp:mbgl-headless',
        '../deps/gtest/gtest.gyp:gtest'
      ],
      'sources': [
        'fixtures/main.cpp',
        'fixtures/util.cpp',
        'fixtures/fixture_log.cpp',

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

        'storage/storage.cpp',
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
      'libraries': [
        '<@(uv_static_libs)',
        '<@(glfw3_static_libs)',
        '<@(sqlite3_static_libs)',
        '<@(zlib_static_libs)',
        '<@(curl_static_libs)',
      ],
      'variables': {
        'cxxflags': [
          '<@(uv_cflags)',
          '-I<(boost_root)/include',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(glfw3_ldflags)',
          '<@(sqlite3_ldflags)',
          '<@(zlib_ldflags)',
          '<@(curl_ldflags)',
        ],
      },
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [ '<@(cxxflags)' ],
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cxxflags)' ],
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          },
        }, {
         'cflags': [ '<@(cxxflags)' ],
         'cxxflags': [ '<@(cxxflags)' ],
         'libraries': [ '<@(ldflags)' ],
        }],
      ],
    },
  ]
}
