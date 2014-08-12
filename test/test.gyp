{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
        'target_name': 'link_gl',
        'type': 'none',
        'direct_dependent_settings': {
            'conditions': [
                ['OS == "mac"', {
                    'link_settings': {
                        'libraries': [
                            '-framework OpenGL',
                        ],
                    },
                    'xcode_settings': {
                        'OTHER_LDFLAGS': [
                            '-framework OpenGL',
                        ],
                    },
                }, {
                    'link_settings': {
                        'libraries': [
                          '<@(glfw3_libraries)', # This is a hack since we're not actually using GLFW
                          '-lboost_regex',
                        ],
                    },
                }],
            ],
        },
    },
    {
        'target_name': 'link_curl',
        'type': 'none',
        'direct_dependent_settings': {
            'conditions': [
                ['OS == "mac"', {
                    'xcode_settings': {
                        'OTHER_CPLUSPLUSFLAGS':[
                            '<@(curl_cflags)',
                        ],
                        'OTHER_LDFLAGS': [
                            '<@(curl_libraries)',
                        ],
                    },
                }, {
                    'cflags': [
                        '<@(curl_cflags)',
                    ],
                    'link_settings': {
                        'libraries': [
                            '<@(curl_libraries)',
                        ],
                    },
                }],
            ],
        },
    },
    {
        "target_name": "rotation_range",
        "product_name": "test_rotation_range",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./rotation_range.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "clip_ids",
        "product_name": "test_clip_ids",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./clip_ids.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "enums",
        "product_name": "test_enums",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./enums.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "style_parser",
        "product_name": "test_style_parser",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./style_parser.cpp",
            "./fixtures/fixture_log.hpp",
            "./fixtures/fixture_log.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
            "../mapboxgl.gyp:copy_fixtures",
        ]
    },
    {
        "target_name": "variant",
        "product_name": "test_variant",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./variant.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "comparisons",
        "product_name": "test_comparisons",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./comparisons.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "tile",
        "product_name": "test_tile",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./tile.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "functions",
        "product_name": "test_functions",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./functions.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
        ]
    },
    {
        "target_name": "headless",
        "product_name": "test_headless",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./headless.cpp",
            "../common/headless_view.hpp",
            "../common/headless_view.cpp",
            "./fixtures/fixture_request.cpp",
            "./fixtures/fixture_log.hpp",
            "./fixtures/fixture_log.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../mapboxgl.gyp:mapboxgl",
            "link_gl",
        ]
    },
    {
        "target_name": "test",
        "type": "none",
        "dependencies": [
          "rotation_range",
          "clip_ids",
          "enums",
          "variant",
          "tile",
          "functions",
          "headless",
          "style_parser",
          "comparisons",
        ],
    }
  ]
}
