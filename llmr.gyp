{
  'includes': [
    './common.gypi',
    './config.gypi'
  ],
  'targets': [
    {
      'target_name': 'llmr',
      'type': 'static_library',
      'sources': [
        '<!@(find src -name "*.cpp")',
        '<!@(find include -name "*.hpp")'
      ],
      'mac_framework_headers': [
        '<!@(find include -name "*.hpp")'
      ],
      'xcode_settings': {
        'PUBLIC_HEADERS_FOLDER_PATH': 'include',
      },
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