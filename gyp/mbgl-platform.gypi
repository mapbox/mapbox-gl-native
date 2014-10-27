{
  'conditions': [
    ['platform == "osx"', {
      'includes': ['mbgl-osx.gypi'],
      'variables': { 'platform_library': 'mbgl-osx' },
    }],
    ['platform == "ios"', {
      'includes': ['mbgl-ios.gypi'],
      'variables': { 'platform_library': 'mbgl-ios' },
    }],
    ['platform == "linux"', {
      'includes': ['mbgl-linux.gypi'],
      'variables': { 'platform_library': 'mbgl-linux' },
    }],
  ],
}
