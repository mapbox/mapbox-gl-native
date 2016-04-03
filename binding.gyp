{
  'variables': {
    'loop_lib': 'uv',
    'headless_lib': 'glx',
  },
  'conditions': [
    ['OS == "mac"', {
      'variables': {
        'headless_lib': 'cgl',
      }
    }],
  ],
  'includes': [
    'mbgl.gypi',
  ],
  'targets': [
    {
      'target_name': '<(module_name)',
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        '<(node_root_dir)/include/node',
        "<!(node -e \"require('nan')\")",
        'include',
        'src', # TODO: eliminate
      ],

      'sources': [
        'platform/node/src/node_mapbox_gl_native.cpp',
        'platform/node/src/node_log.hpp',
        'platform/node/src/node_log.cpp',
        'platform/node/src/node_map.hpp',
        'platform/node/src/node_map.cpp',
        'platform/node/src/node_request.hpp',
        'platform/node/src/node_request.cpp',
        'platform/node/src/util/async_queue.hpp',
      ],

      'conditions': [
        ['OS == "mac"', {
          'sources': [
            'platform/darwin/src/log_nslog.mm',
            'platform/darwin/src/string_nsstring.mm',
            'platform/darwin/src/image.mm',
            'platform/darwin/src/nsthread.mm',
          ]
        }, {
          'sources': [
            'platform/default/log_stderr.cpp',
            'platform/default/string_stdlib.cpp',
            'platform/default/thread.cpp',
            'platform/default/image.cpp',
            'platform/default/webp_reader.cpp',
            'platform/default/png_reader.cpp',
            'platform/default/jpeg_reader.cpp',
          ],

          'cflags_cc': [
            '<@(boost_cflags)',
            '<@(nunicode_cflags)',
            '<@(webp_cflags)',
            '<@(libpng_cflags)',
            '<@(libjpeg-turbo_cflags)',
          ],

          'link_settings': {
            'libraries': [
              '<@(nunicode_ldflags)',
              '<@(nunicode_static_libs)',
              '<@(webp_ldflags)',
              '<@(webp_static_libs)',
              '<@(libpng_ldflags)',
              '<@(libpng_static_libs)',
              '<@(libjpeg-turbo_ldflags)',
              '<@(libjpeg-turbo_static_libs)',
            ],
          },
        }]
      ],

      'xcode_settings': {
        'OTHER_LDFLAGS':[
          '-Wl,-bind_at_load'
        ],
      },
    },
    {
      'target_name': 'action_after_build',
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
