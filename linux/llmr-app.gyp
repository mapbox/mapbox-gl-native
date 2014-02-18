{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
        "target_name": "linuxapp",
        "product_name": "llmr",
        "type": "executable",
        "sources": [
            "./main.cpp",
            "./settings.cpp",
            "./settings.hpp"
        ],
        'product_extension': 'app',
        'link_settings': {
          'libraries': [
            '<@(glfw3_libraries)',
            '-lcurl'
          ],
        },
        'cflags': [
            '<@(png_cflags)',
            '<@(glfw3_cflags)'
        ],
        "dependencies": [
            "../llmr.gyp:llmr-osx"
        ]
    }
  ]
}
