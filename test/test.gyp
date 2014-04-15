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
        "target_name": "headless",
        "product_name": "test_headless",
        "type": "executable",
        "libraries": [
            "-lpthread",
        ],
        "sources": [
            "./main.cpp",
            "./headless.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../llmr.gyp:llmr-x86",
            "link_gl",
        ]
    },
    {
        "target_name": "test",
        "type": "none",
        "dependencies": [
          "rotation_range"
        ],
    }
  ]
}
