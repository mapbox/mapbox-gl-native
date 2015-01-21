{
  'targets': [
    { 'target_name': 'test_miscellaneous',
      'type': 'executable',
      'dependencies': [
        'test_base',
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../mapboxgl.gyp:mbgl-headless',
        '../deps/gtest/gtest.gyp:gtest'
      ],
      'sources': [
        'main.cpp',
        'clip_ids.cpp',
        'comparisons.cpp',
        'enums.cpp',
        'functions.cpp',
        'rotation_range.cpp',
        'style_parser.cpp',
        'text_conversions.cpp',
        'tile.cpp',
        'variant.cpp',
      ],
    },
  ],
}
