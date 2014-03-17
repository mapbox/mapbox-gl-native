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
        "sources": [
            "./main.cpp",
            "./rotation_range.cpp",
        ],
        "dependencies": [
            "../deps/gtest/gtest.gyp:gtest",
            "../llmr.gyp:llmr-x86"
        ]
    },
  ]
}
