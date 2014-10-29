{
  'conditions': [
    ['install_prefix != ""', {
      'targets': [
        { 'target_name': 'install',
          'type': 'none',
          'hard_dependency': 1,
          'dependencies': [
            'mbgl-core',
            'mbgl-standalone',
            'mbgl-headless',
            'mbgl-<(platform)',
          ],
          'copies': [
            { 'files': [ '<(standalone_product_dir)/libmbgl.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-core.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-headless.a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '<(PRODUCT_DIR)/libmbgl-<(platform).a' ], 'destination': '<(install_prefix)/lib' },
            { 'files': [ '../include/mbgl' ], 'destination': '<(install_prefix)/include' }
          ],
        },
      ]
    }],
  ],
}
