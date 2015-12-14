{
  'targets': [
    {
      'target_name': 'shaders',
      'type': 'none',
      'hard_dependency': 1,

      'sources': [
        '<!@(find ../src/mbgl/shader -name "*.glsl")'
      ],

      'rules': [
        {
          'rule_name': 'Build Shaders',
          'message': 'Building shader',
          'extension': 'glsl',
          'inputs': [ '../scripts/build-shaders.py' ],
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/<(RULE_INPUT_ROOT).hpp' ],
          'action': [ '../scripts/build-shaders.py', '<(RULE_INPUT_PATH)', '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/<(RULE_INPUT_ROOT).hpp' ],
          'process_outputs_as_sources': 1,
        }
      ],

      'direct_dependent_settings': {
        'include_dirs': [
          '<(SHARED_INTERMEDIATE_DIR)/include',
        ]
      }

    },
  ]
}
