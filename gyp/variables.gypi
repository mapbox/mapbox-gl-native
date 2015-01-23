{
  'variables': {
    'core_library%': 'mbgl-core',
    'headless_library%': 'mbgl-headless',
  },
  'conditions': [
    ['platform == "osx"', {
      'variables': {
        'platform_library%': 'mbgl-osx',
        'storage_library%': 'mbgl-storage-cocoa',
      },
    }],
    ['platform == "ios"', {
      'variables': {
        'platform_library%': 'mbgl-ios',
        'storage_library%': 'mbgl-storage-cocoa',
      },
    }],
    ['platform == "linux"', {
      'variables': {
        'platform_library%': 'mbgl-linux',
        'storage_library%': 'mbgl-storage-curl',
      },
    }],
    ['platform == "android"', {
      'variables': {
        'platform_library%': 'mbgl-android',
        'storage_library%': 'mbgl-storage-curl',
      },
    }],
  ],
}