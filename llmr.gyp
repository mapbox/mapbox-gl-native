{
  'targets': [
    {
      'target_name': 'shaders',
      'type': 'none',
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [
            '<!@(find src -name "*.glsl")'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/include/llmr/shader/shaders.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders.cpp'
          ],
          'action': ['bin/build-shaders.js', '<(shader_type)', '<(SHARED_INTERMEDIATE_DIR)'],
        }
      ]
    },
    {
      'target_name': 'build_stylesheet',
      'type': 'none',
      'actions': [
        {
          'action_name': 'Build Stylesheet',
          'inputs': [
            'bin/style.js'
          ],
          'outputs': [
            'include/llmr/style/resources.hpp',
            'src/style/resources.cpp'
          ],
          'action': ['bin/build-style.js']
        }
      ],
    },
    {
      'target_name': 'llmr',
      'product_name': 'llmr',
      'type': '<(library)',
      'dependencies': [
          'build_stylesheet',
          'shaders',
      ],
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find src -name "*.c")',
        '<!@(find src -name "*.h")',
        '<!@(find include -name "*.hpp")',
        '<!@(find include -name "*.h")',
        '<!@(find src -name "*.glsl")',
        '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders.cpp',
        '<(SHARED_INTERMEDIATE_DIR)/include/llmr/shader/shaders.hpp',
        'bin/style.js'
      ],
      'include_dirs':[
          './include',
          '<(SHARED_INTERMEDIATE_DIR)/include',
      ],
      'cflags': [
          '<@(uv_cflags)',
          '<@(png_cflags)',
          '-I<(boost_root)/include',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
            '<@(uv_cflags)',
            '<@(png_cflags)',
            '-I<(boost_root)/include',
        ],
      },
      'direct_dependent_settings': {
          'include_dirs': [
              './include'
          ],
          'conditions': [
            ['OS == "mac"', {
              'xcode_settings': {
                'OTHER_CPLUSPLUSFLAGS': [
                    '<@(uv_cflags)',
                    '<@(png_cflags)',
                ],
                'OTHER_LDFLAGS': [
                    '<@(uv_libraries)',
                    '<@(png_libraries)',
                ],
              }
            }, {
              'cflags': [
                  '<@(uv_cflags)',
                  '<@(png_cflags)',
              ],
              'libraries': [
                '<@(uv_libraries)',
                '<@(png_libraries)',
              ]
            }]
          ]
      }
    },
  ]
}