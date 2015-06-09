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
          'inputs': ['<!@(pwd)/../test'],
          'outputs': ['<(PRODUCT_DIR)/TEST_DATA'], # symlinks the test dir into TEST_DATA
          'action': ['ln', '-s', '-f', '-n', '<@(_inputs)', '<@(_outputs)' ],
        }
      ],
    },
    { 'target_name': 'test',
      'type': 'executable',
      'include_dirs': [ '../include', '../src', '../platform/default' ],
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
        'fixtures/fixture_log_observer.hpp',
        'fixtures/fixture_log_observer.cpp',

        'api/api_misuse.cpp',
        'api/repeated_render.cpp',
        'api/set_style.cpp',

        'headless/headless.cpp',

        'miscellaneous/assert.cpp',
        'miscellaneous/clip_ids.cpp',
        'miscellaneous/binpack.cpp',
        'miscellaneous/bilinear.cpp',
        'miscellaneous/comparisons.cpp',
        'miscellaneous/enums.cpp',
        'miscellaneous/functions.cpp',
        'miscellaneous/geo.cpp',
        'miscellaneous/map.cpp',
        'miscellaneous/map_context.cpp',
        'miscellaneous/mapbox.cpp',
        'miscellaneous/merge_lines.cpp',
        'miscellaneous/style_parser.cpp',
        'miscellaneous/text_conversions.cpp',
        'miscellaneous/thread.cpp',
        'miscellaneous/tile.cpp',
        'miscellaneous/transform.cpp',
        'miscellaneous/variant.cpp',

        'storage/storage.hpp',
        'storage/storage.cpp',
        'storage/cache_response.cpp',
        'storage/cache_revalidate.cpp',
        'storage/database.cpp',
        'storage/directory_reading.cpp',
        'storage/file_reading.cpp',
        'storage/http_cancel.cpp',
        'storage/http_coalescing.cpp',
        'storage/http_error.cpp',
        'storage/http_header_parsing.cpp',
        'storage/http_issue_1369.cpp',
        'storage/http_load.cpp',
        'storage/http_other_loop.cpp',
        'storage/http_reading.cpp',

        'style/mock_file_source.cpp',
        'style/mock_file_source.hpp',
        'style/mock_view.hpp',
        'style/pending_resources.cpp',
        'style/resource_loading.cpp',
      ],
      'libraries': [
        '<@(uv_static_libs)',
        '<@(sqlite3_static_libs)',
      ],
      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
          '<@(opengl_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite3_cflags)',
        ],
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(sqlite3_ldflags)',
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
