{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': '<(module_name)',
      'dependencies': [
        '../../mbgl.gyp:core',
        '../../mbgl.gyp:platform-<(platform_lib)',
        '../../mbgl.gyp:headless-<(headless_lib)',
      ],

      'include_dirs': [ "<!(node -e \"require('nan')\")" ],

      'sources': [
        'src/node_mapbox_gl_native.cpp',
        'src/node_log.hpp',
        'src/node_log.cpp',
        'src/node_file_source.hpp',
        'src/node_file_source.cpp',
        'src/node_map.hpp',
        'src/node_map.cpp',
        'src/node_request.hpp',
        'src/node_request.cpp',
        'src/util/async_queue.hpp',
      ],

      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'CLANG_CXX_LIBRARY': 'libc++',
            'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'OTHER_CPLUSPLUSFLAGS': [
              '-std=c++1y',
              '-Werror',
              '-Wall',
              '-Wextra',
              '-Wshadow',
              '-Wno-variadic-macros',
              '-Wno-error=unused-parameter',
              '-frtti',
              '-fexceptions',
            ],
            'GCC_WARN_PEDANTIC': 'YES',
            'GCC_WARN_UNINITIALIZED_AUTOS': 'YES_AGGRESSIVE',
            'MACOSX_DEPLOYMENT_TARGET': '10.9',
          },
        }, {
          'cflags_cc': [
            '-std=c++14',
            '-Werror',
            '-Wall',
            '-Wextra',
            '-Wno-variadic-macros',
            '-Wno-error=unused-parameter',
            '-frtti',
            '-fexceptions',
          ],
          'libraries': [ '<@(glfw3_ldflags)' ],
        }],
        ['OS=="linux"', {
          'cflags_cc': [
            '-Wno-unknown-pragmas', # We are using '#pragma mark', but it is only available on Darwin.
          ],
        }],
      ],
    },

    { 'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ '<(module_name)' ],
      'copies': [
        {
          'files': [ '<(PRODUCT_DIR)/<(module_name).node' ],
          'destination': '<(module_path)'
        }
      ]
    }
  ]
}
