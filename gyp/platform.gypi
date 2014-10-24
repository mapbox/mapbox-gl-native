{
  'conditions': [
    ['platform == "osx"', {
      'includes': ['platform-osx.gypi'],
      'variables': { 'platform_library': 'mapboxgl-osx' },
    }],
    ['platform == "ios"', {
      'includes': ['platform-ios.gypi'],
      'variables': { 'platform_library': 'mapboxgl-ios' },
    }],
    ['platform == "linux"', {
      'includes': ['platform-linux.gypi'],
      'variables': { 'platform_library': 'mapboxgl-linux' },
    }],
  ],
}
