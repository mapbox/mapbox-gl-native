{
  'conditions': [
    ['install_prefix != ""', {
      'targets': [
        { 'target_name': 'install',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'mbgl',
            'mbgl-headless',
            'mbgl-<(platform)',
          ],
          'copies': [
            { 'files': [ '<(PRODUCT_DIR)/libmbgl.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-headless.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-<(platform).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '../include/mbgl' ], 'destination': '<(install_prefix)/include' }
          ],
          'actions': [
              { 'action_name': 'mbgl-config',  
                'inputs': [
                    '../utils/mbgl-config/mbgl-config.template.sh',
                    '../utils/mbgl-config/build.sh',
                ],
                'outputs': [
                    '<(install_prefix)/bin/mbgl-config',
                ],
                'action': [
                    './utils/mbgl-config/build.sh',
                    '<(install_prefix)',
                    '<(platform)',
                ]
              }
          ]
        },
      ]
    }],
  ],
}
