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
      ],
      'sources': [
        'fixtures/main.cpp',
        'fixtures/mock_file_source.cpp',
        'fixtures/mock_file_source.hpp',
        'fixtures/mock_view.hpp',
        'fixtures/util.hpp',
        'fixtures/util.cpp',
        'fixtures/fixture_log_observer.hpp',
        'fixtures/fixture_log_observer.cpp',

        'miscellaneous/assert.cpp',

        'annotations/sprite_atlas.cpp',
        'annotations/sprite_image.cpp',
        'annotations/sprite_store.cpp',
        'annotations/sprite_parser.cpp',

        'api/annotations.cpp',
        'api/api_misuse.cpp',
        'api/repeated_render.cpp',
        'api/set_style.cpp',


        'miscellaneous/clip_ids.cpp',
        'miscellaneous/binpack.cpp',
        'miscellaneous/bilinear.cpp',
        'miscellaneous/comparisons.cpp',
        'miscellaneous/custom_sprites.cpp',
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
        'miscellaneous/work_queue.cpp',
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

        'style/glyph_store.cpp',
        'style/pending_resources.cpp',
        'style/resource_loading.cpp',
        'style/sprite.cpp',
      ],
      'libraries': [
        '<@(gtest_static_libs)',
        '<@(libuv_static_libs)',
        '<@(sqlite_static_libs)',
      ],
      'variables': {
        'cflags_cc': [
          '<@(gtest_cflags)',
          '<@(libuv_cflags)',
          '<@(opengl_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(variant_cflags)',
          '<@(rapidjson_cflags)',
        ],
        'ldflags': [
          '<@(gtest_ldflags)',
          '<@(libuv_ldflags)',
          '<@(sqlite_ldflags)',
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
