{
  'targets': [
    { 'target_name': 'mbgl',
      'dependencies': [
        './<(mbgl)/mbgl.gyp:core',
        './<(mbgl)/mbgl.gyp:platform-<(platform_lib)',
        './<(mbgl)/mbgl.gyp:headless-<(headless_lib)',
      ],

      'include_dirs': [ "<!(node -e \"require('nan')\")" ],

      'sources': [
        'src/mbgl.cpp',
        'src/compress_png.hpp',
        'src/compress_png.cpp',
        'src/node_file_source.hpp',
        'src/node_file_source.cpp',
        'src/node_map.hpp',
        'src/node_map.cpp',
        'src/node_request.hpp',
        'src/node_request.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '-Wno-unused-parameter',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            "CLANG_CXX_LIBRARY": "libc++",
            "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
            "GCC_VERSION": "com.apple.compilers.llvm.clang.1_0",
            'MACOSX_DEPLOYMENT_TARGET': '10.7',
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          }
        }, {
          'cflags_cc': [
            '-std=c++11',
            '-fexceptions',
            '<@(cflags_cc)',
          ],
          'libraries': [ '<@(glfw3_ldflags)' ],
        }]
      ],
    },

    { 'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ 'mbgl' ],
      'copies': [
        {
          'files': [ '<(PRODUCT_DIR)/mbgl.node' ],
          'destination': 'lib'
        }
      ]
    }
  ]
}
