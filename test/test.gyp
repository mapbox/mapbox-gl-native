{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
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
        "target_name": "test",
        "type": "none",
        "dependencies": [
          "rotation_range"
        ],
    }
  ]
}
