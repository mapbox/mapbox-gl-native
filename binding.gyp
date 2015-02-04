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
        'src/node_file_source.hpp',
        'src/node_file_source.cpp',
        'src/node_map.hpp',
        'src/node_map.cpp',
        'src/node_map_render_worker.hpp',
        'src/node_map_render_worker.cpp',
        'src/node_request.hpp',
        'src/node_request.cpp',
      ],

      'variables': {
        'cflags_cc': [
          '-std=c++11',
          '-stdlib=libc++',
          '-Wno-unused-parameter'
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'MACOSX_DEPLOYMENT_TARGET': '10.7',
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }, {
          'cflags_cc': [
            '<@(cflags_cc)',
            '-fexceptions',
          ],
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