{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'symlink_TEST_DATA',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'Symlink Fixture Directory',
          'inputs': ['<!@(pwd)'],
          'outputs': ['<(PRODUCT_DIR)/TEST_DATA'], # symlinks the test dir into TEST_DATA
          'action': ['ln', '-s', '-f', '-n', '<@(_inputs)', '<@(_outputs)' ],
        }
      ],
    },
    { 'target_name': 'test',
      'type': 'executable',
      'include_dirs': [ '../include', '../src' ],
      'dependencies': [
        'symlink_TEST_DATA',
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
        '../mbgl.gyp:headless-<(headless_lib)',
        '../deps/gtest/gtest.gyp:gtest'
      ],
      'sources': [
        'fixtures/main.cpp',
        'fixtures/util.hpp',
        'fixtures/util.cpp',
        'fixtures/fixture_log.hpp',
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

        'storage/storage.hpp',
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
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(boost_cflags)',
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
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
         'libraries': [ '<@(ldflags)' ],
        }],
      ],
    },
  ]
}
