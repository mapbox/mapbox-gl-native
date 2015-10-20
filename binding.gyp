{
  'includes': [
    'gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': '<(module_name)',
      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:headless-<(headless_lib)',
      ],

      'include_dirs': [
        "<!(node -e \"require('nan')\")",
      ],

      'sources': [
        'platform/node/src/node_mapbox_gl_native.cpp',
        'platform/node/src/node_log.hpp',
        'platform/node/src/node_log.cpp',
        'platform/node/src/node_file_source.hpp',
        'platform/node/src/node_file_source.cpp',
        'platform/node/src/node_map.hpp',
        'platform/node/src/node_map.cpp',
        'platform/node/src/node_request.hpp',
        'platform/node/src/node_request.cpp',
        'platform/node/src/util/async_queue.hpp',
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
