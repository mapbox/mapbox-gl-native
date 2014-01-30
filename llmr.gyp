{
  'includes': [
    'common.gypi',
    'config.gypi'
  ],
  'targets': [
    {
      'target_name': 'llmr',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")'
      ],
      'include_dirs':[
          './include',
          '<(boost_root)/include'
      ],
      'direct_dependent_settings': {
          'include_dirs':[
              './include',
              '<(boost_root)/include'
          ],
      }
    }
  ]
}