{
  'targets': [
    {
      'target_name': 'shaders',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'npm_install'
      ],
      'actions': [
        {
          'action_name': 'Build Shaders',
          'inputs': [
            '<!@(find src -name "*.glsl")'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/shaders.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gl.cpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gles2.cpp',
          ],
          'action': ['<@(node)', 'bin/build-shaders.js', '<(SHARED_INTERMEDIATE_DIR)'],
        }
      ],
      'direct_dependent_settings': {
        'sources': [
            '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/shaders.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gl.cpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/shader/shaders_gles2.cpp'
        ],
        'include_dirs': [
          '<(SHARED_INTERMEDIATE_DIR)/include/',
        ]
      }
    },
  ]
}
