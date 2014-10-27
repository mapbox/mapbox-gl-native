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
      'direct_dependent_settings': {
        'mac_bundle_resources': [ '../styles' ],
      }
    },
    {
      'target_name': 'copy_styles',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [ 'touch_styles' ], # required for xcode http://openradar.appspot.com/7232149
      'copies': [{
        'files': [ '../styles' ],
        'destination': '<(PRODUCT_DIR)'
      }],
    },
  ]
}
