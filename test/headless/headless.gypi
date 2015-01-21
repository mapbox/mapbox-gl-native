{
  'targets': [
    { 'target_name': 'test_headless',
      'type': 'executable',
      'dependencies': [
        'test_base',
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../mapboxgl.gyp:mbgl-headless',
        '../deps/gtest/gtest.gyp:gtest',
      ],
      'sources': [
        'headless.cpp',
      ],
    },
  ],
}
