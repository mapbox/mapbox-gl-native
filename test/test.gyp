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
                }, {
                    # TODO: add OpenGL link settings for linux
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
            "../llmr.gyp:llmr-x86"
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
            "../llmr.gyp:llmr-x86"
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
            "../common/curl_request.hpp",
            "../common/curl_request.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../llmr.gyp:llmr-x86",
            "link_gl",
            "link_curl",
        ]
    },
    {
        "target_name": "test",
        "type": "none",
        "dependencies": [
          "rotation_range",
          "variant",
        ],
    }
  ]
}
