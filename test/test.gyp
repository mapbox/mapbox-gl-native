{
  'includes': [
    '../gyp/common.gypi',
    '../gyp/version.gypi',
    '../gyp/mbgl-platform.gypi',
  ],
  'variables' : {
    'ldflags': [
      '<@(uv_ldflags)',
      '<@(sqlite3_static_libs)',
      '<@(sqlite3_ldflags)',
      '<@(curl_ldflags)',
      '<@(png_ldflags)',
    ],
  },
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)'] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)'] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone'
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
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
        '../mapboxgl.gyp:mbgl-standalone',
      ],
      'include_dirs': [ '../src' ],
      'conditions': [
        ['OS == "mac"', { 'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
        }, {
          'libraries': [ '<@(ldflags)' ],
        }]
      ]
    },
    { 'target_name': 'headless',
      'product_name': 'test_headless',
      'type': 'executable',
      'sources': [
        './headless.cpp',
        './fixtures/fixture_log.cpp',
      ],
      'conditions': [
        # add libuv include path and OpenGL libs
        ['OS == "mac"',
          {
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': ['<@(uv_cflags)','<@(png_cflags)'],
              'OTHER_LDFLAGS': ['<@(glfw3_ldflags)', '<@(ldflags)'],
            },
          },
          {
            'cflags': ['<@(uv_cflags)','<@(png_cflags)'],
            'libraries': ['<@(glfw3_ldflags)', '<@(ldflags)'],
          }],
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl-standalone',
        '../mapboxgl.gyp:mbgl-headless',
        '<(platform_library)',
      ],
      'include_dirs': [ '../src' ]
    },
    { 'target_name': 'text_conversions',
      'product_name': 'test_text_conversions',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './text_conversions.cpp',
      ],
      'dependencies': [
        '../deps/gtest/gtest.gyp:gtest',
        '../mapboxgl.gyp:mbgl-standalone',
        '<(platform_library)',
      ],
      'include_dirs': [ '../src' ],
      'variables': {
        'cflags_cc': [
          '-I<(boost_root)/include',
        ]
      },
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_LDFLAGS': [ '<@(ldflags)', '-framework Foundation' ]
          },
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'libraries': [ '<@(ldflags)'],
        }]
      ]
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
        'text_conversions',
      ],
    }
  ]
}
