{
  'targets': [
    {
      'target_name': 'copy_fixtures',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'bundle_styles'
      ],
      'copies': [
        {
          'files': [ '../styles' ],
          'destination': '../test/fixtures/style_parser'
        }
      ]
    },
  ]
}
