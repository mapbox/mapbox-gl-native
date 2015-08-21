{
  'includes': [ 'platform-darwin.gypi' ],
  'targets': [
    { 'target_name': 'platform-osx',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'platform-darwin',
      ],
      'link_settings': {
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            '-framework OpenGL',
            '-framework ApplicationServices',
          ],
        },
      },
    },
  ],
}
