{
  'targets': [
    {
      'target_name': 'version',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'Build Version File',
          'inputs': [
            '../scripts/build-version.py',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp',
            '<(SHARED_INTERMEDIATE_DIR)/src/mbgl/util/mbgl_version.cpp',
          ],
          'action': ['<@(python)', 'scripts/build-version.py', '<(SHARED_INTERMEDIATE_DIR)', '<!@(git describe --tags --always --abbrev=8)', '<!@(git rev-parse --short=8 HEAD)'],
        }
      ],
      'direct_dependent_settings': {
        'sources': [
          '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp',
          '<(SHARED_INTERMEDIATE_DIR)/src/mbgl/util/mbgl_version.cpp',
        ],
        'include_dirs': [
          '<(SHARED_INTERMEDIATE_DIR)/include',
        ]
      }
    },
  ]
}
