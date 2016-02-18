{
  'includes': [
    '../gyp/common.gypi',
  ],

  'targets': [
    { 'target_name': 'test',
      'type': 'executable',

      'include_dirs': [
        '../src',
        '../platform/default',
        'include',
      ],

      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
        'mbgl.gyp:headless-<(headless_lib)',
      ],

      'sources': [
        # Test helper files
        'src/stub_file_source.cpp',
        'include/mbgl/test/stub_file_source.hpp',
        'include/mbgl/test/mock_view.hpp',
        'include/mbgl/test/util.hpp',
        'src/util.cpp',
        'include/mbgl/test/fixture_log_observer.hpp',
        'src/fixture_log_observer.cpp',

        'util/assert.cpp',
        'util/async_task.cpp',
        'util/clip_ids.cpp',
        'util/geo.cpp',
        'util/image.cpp',
        'util/mapbox.cpp',
        'util/math.cpp',
        'util/merge_lines.cpp',
        'util/run_loop.cpp',
        'util/text_conversions.cpp',
        'util/thread.cpp',
        'util/thread_local.cpp',
        'util/tile_cover.cpp',
        'util/timer.cpp',
        'util/token.cpp',
        'util/work_queue.cpp',

        'api/annotations.cpp',
        'api/api_misuse.cpp',
        'api/repeated_render.cpp',
        'api/render_missing.cpp',
        'api/set_style.cpp',
        'api/custom_layer.cpp',
        'api/offline.cpp',

        'geometry/binpack.cpp',

        'map/map.cpp',
        'map/map_context.cpp',
        'map/tile.cpp',
        'map/transform.cpp',

        'storage/storage.hpp',
        'storage/storage.cpp',
        'storage/default_file_source.cpp',
        'storage/offline.cpp',
        'storage/offline_database.cpp',
        'storage/offline_download.cpp',
        'storage/asset_file_source.cpp',
        'storage/headers.cpp',
        'storage/http_cancel.cpp',
        'storage/http_error.cpp',
        'storage/http_expires.cpp',
        'storage/http_header_parsing.cpp',
        'storage/http_issue_1369.cpp',
        'storage/http_load.cpp',
        'storage/http_other_loop.cpp',
        'storage/http_retry_network_status.cpp',
        'storage/http_reading.cpp',
        'storage/http_timeout.cpp',
        'storage/resource.cpp',

        'style/glyph_store.cpp',
        'style/source.cpp',
        'style/style.cpp',
        'style/style_layer.cpp',
        'style/comparisons.cpp',
        'style/functions.cpp',
        'style/style_parser.cpp',
        'style/variant.cpp',

        'sprite/sprite_atlas.cpp',
        'sprite/sprite_image.cpp',
        'sprite/sprite_parser.cpp',
        'sprite/sprite_store.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '<@(gtest_cflags)',
          '<@(opengl_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(geojsonvt_cflags)',
          '<@(rapidjson_cflags)',
          '<@(pixelmatch_cflags)',
          '<@(variant_cflags)',
        ],
        'ldflags': [
          '<@(gtest_ldflags)',
          '<@(sqlite_ldflags)',
        ],
        'libraries': [
          '<@(gtest_static_libs)',
          '<@(sqlite_static_libs)',
          '<@(geojsonvt_static_libs)',
        ],
      },

      'conditions': [
        ['host == "ios"', {
          'product_name': 'ios-test',
          # iOS tests
          'includes': [
            '../gyp/target-ios-bundle.gypi',
          ],
          'sources': [
            'src/main.mm',
            '../src/mbgl/util/premultiply.cpp',
          ],
          'xcode_settings': {
            'INFOPLIST_FILE': '../test/src/app-info.plist',
          },
          'copies': [
            { 'destination': '<(PRODUCT_DIR)/$(WRAPPER_NAME)/test',
              # Hack: When a filename begins with a $, gyp doesn't prepend $(SRCROOT)
              'files': [ '$()../test/fixtures' ],
            },
          ]
        }, {
          # non-iOS tests
          'sources': [
            'src/main.cpp',
          ]
        }],
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
    },
  ]
}
