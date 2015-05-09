{
  'targets': [
    {
      'target_name': 'touch_styles',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'Touch Stylesheet Directory',
          'inputs': ['../styles'],
          'outputs': ['<(SHARED_INTERMEDIATE_DIR)/'], # need to specify a distinct directory
          'action': ['touch', '../styles'],
        }
      ],
    },
    {
      'target_name': 'bundle_styles',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [ 'touch_styles' ], # required for xcode http://openradar.appspot.com/7232149
      'conditions': [
        ['platform_lib == "osx" or platform_lib == "ios"', {
          'direct_dependent_settings': {
            'mac_bundle_resources': [ '../styles/styles' ],
          }
        }, {
          'direct_dependent_settings': {
            'copies': [{ 'files': [ '../styles/styles' ], 'destination': '<(PRODUCT_DIR)' }],
          }
        }]
      ],
    }
  ]
}
