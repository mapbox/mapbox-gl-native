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
      'target_name': 'bundle_styles', # use this only for targets that create an App bundle
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [ 'touch_styles' ], # required for xcode http://openradar.appspot.com/7232149
      'direct_dependent_settings': {
        'mac_bundle_resources': [ '../styles/styles' ],
      }
    },
    {
      'target_name': 'copy_styles', # use this only for targets that don't create an App bundle
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [ 'touch_styles' ],
      'direct_dependent_settings': {
        'copies': [{ 'files': [ '../styles/styles' ], 'destination': '<(PRODUCT_DIR)' }],
      }
    },
  ]
}
