{
  'targets': [
    {
      'target_name': 'test-lib',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        '../include',
        '../src',
        '../platform/default',
        'include',
        'src',
      ],

      'sources': [
        'util/async_task.cpp',
        'util/geo.cpp',
        'util/image.cpp',
        'util/mapbox.cpp',
        'util/memory.cpp',
        'util/merge_lines.cpp',
        'util/run_loop.cpp',
        'util/number_conversions.cpp',
        'util/text_conversions.cpp',
        'util/thread.cpp',
        'util/thread_local.cpp',
        'util/tile_cover.cpp',
        'util/timer.cpp',
        'util/token.cpp',
        'util/work_queue.cpp',
        'util/projection.cpp',

        'algorithm/covered_by_children.cpp',
        'algorithm/generate_clip_ids.cpp',
        'algorithm/mock.hpp',
        'algorithm/update_renderables.cpp',

        'api/annotations.cpp',
        'api/api_misuse.cpp',
        'api/repeated_render.cpp',
        'api/render_missing.cpp',
        'api/set_style.cpp',
        'api/custom_layer.cpp',

        'geometry/binpack.cpp',

        'gl/object.cpp',

        'map/map.cpp',
        'map/transform.cpp',

        'math/minmax.cpp',
        'math/clamp.cpp',

        'text/quads.cpp',

        'tile/geometry_tile_data.cpp',
        'tile/tile_id.cpp',

        'storage/offline.cpp',
        'storage/offline_database.cpp',
        'storage/offline_download.cpp',
        'storage/asset_file_source.cpp',
        'storage/default_file_source.cpp',
        'storage/http_file_source.cpp',
        'storage/online_file_source.cpp',
        'storage/headers.cpp',
        'storage/resource.cpp',

        'style/glyph_store.cpp',
        'style/source.cpp',
        'style/style.cpp',
        'style/style_layer.cpp',
        'style/tile_source.cpp',
        'style/filter.cpp',
        'style/functions.cpp',
        'style/style_parser.cpp',

        'sprite/sprite_atlas.cpp',
        'sprite/sprite_image.cpp',
        'sprite/sprite_parser.cpp',
        'sprite/sprite_store.cpp',

        'src/mbgl/test/stub_file_source.hpp',
        'src/mbgl/test/stub_file_source.cpp',
        'src/mbgl/test/util.hpp',
        'src/mbgl/test/util.cpp',
        'src/mbgl/test/fixture_log_observer.hpp',
        'src/mbgl/test/fixture_log_observer.cpp',
        'src/mbgl/test/test.cpp'
      ],

      'variables': {
        'cflags_cc': [
          '<@(gtest_cflags)',
          '<@(opengl_cflags)',
          '<@(protozero_cflags)',
          '<@(boost_cflags)',
          '<@(sqlite_cflags)',
          '<@(geojsonvt_cflags)',
          '<@(rapidjson_cflags)',
          '<@(pixelmatch_cflags)',
          '<@(earcut_cflags)',
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
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],

        #Exclude tests that don't run on android yet
        ['loop_lib == "android"', {
          'sources!': ['util/async_task.cpp'], # Doesn't compile??
          'sources/': [
            ['exclude', 'util/.*(hpp|cpp)$'],
            ['include', 'util/run_loop.cpp'],
            ['include', 'util/async_task.cpp'],
            ['include', 'util/timer.cpp'],
            ['exclude', 'api/.*(hpp|cpp)$'],
            ['exclude', 'gl/object.cpp'],
            ['exclude', 'map/.*(hpp|cpp)$'],
            ['exclude', 'storage/.*(hpp|cpp)$'],
            ['exclude', 'style/.*(hpp|cpp)$'],
            ['exclude', 'sprite/.*(hpp|cpp)$'],
          ]
        }]
      ],
      
      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }],
          
          #Android doesn't support -pthread
          ['loop_lib == "android"', {
            'libraries!': ['<@(gtest_ldflags)']
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          'include',
        ],
        
        # Force the linker to include all the objects from the lib-test archive. Otherwise they'd
        # be discarded because there are no undefined symbols to pull them in, and the resulting
        # executable would run zero tests.
        #
        # Don't do this for android as linking this in a shared library leads to linker
        # errors: "previous definition here"


        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-Wl,-force_load,<(PRODUCT_DIR)/libtest-lib.a',
              ],
            }
          }, {
            'conditions': [
              ['loop_lib != "android"', {
                'link_settings': {
                  'ldflags': [
                    '-Wl,-whole-archive <(PRODUCT_DIR)/libtest-lib.a -Wl,-no-whole-archive',
                  ],
                },
              }]
            ],
          }],
        ]
      },
    },
  ]
}
