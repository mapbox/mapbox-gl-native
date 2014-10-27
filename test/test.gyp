{
  'includes': [
    '../gyp/common.gypi',
    '../gyp/mbgl-platform.gypi',
  ],
  'targets': [
    { 'target_name': 'rotation_range',
      'product_name': 'test_rotation_range',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './rotation_range.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'clip_ids',
      'product_name': 'test_clip_ids',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './clip_ids.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'enums',
      'product_name': 'test_enums',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './enums.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'style_parser',
      'product_name': 'test_style_parser',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './style_parser.cpp',
        './fixtures/fixture_log.hpp',
        './fixtures/fixture_log.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
        '../mapboxgl.gyp:copy_fixtures',
      ]
    },
    { 'target_name': 'variant',
      'product_name': 'test_variant',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './variant.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'comparisons',
      'product_name': 'test_comparisons',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './comparisons.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'tile',
      'product_name': 'test_tile',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './tile.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
      ]
    },
    { 'target_name': 'functions',
      'product_name': 'test_functions',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './functions.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
        ]
    },
    { 'target_name': 'headless',
      'product_name': 'test_headless',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './headless.cpp',
        './fixtures/fixture_request.cpp',
        './fixtures/fixture_log.cpp',
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_display.cpp',
      ],
      'conditions': [
        # add libuv include path and OpenGL libs
        ['OS == "mac"',
          {
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': ['<@(uv_cflags)'],
              'OTHER_LDFLAGS': ['<@(glfw3_libraries)'],
            },
          },
          {
            'cflags': ['<@(uv_cflags)'],
            'libraries': ['<@(glfw3_libraries)'],
          }],
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl',
        '<(platform_library)',
      ],
    },

    # Build all targets
    { 'target_name': 'test',
      'type': 'none',
      'dependencies': [
        'rotation_range',
        'clip_ids',
        'enums',
        'variant',
        'tile',
        'functions',
        'headless',
        'style_parser',
        'comparisons',
      ],
    }
  ]
}
