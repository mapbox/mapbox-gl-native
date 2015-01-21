{
  'targets': [
    { 'target_name': 'test_storage',
      'type': 'executable',
      'dependencies': [
        'test_base',
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-<(platform)',
        '../deps/gtest/gtest.gyp:gtest'
      ],
      'sources': [
        'main.cpp',
        'cache_response.cpp',
        'cache_revalidate.cpp',
        'directory_reading.cpp',
        'file_reading.cpp',
        'http_cancel.cpp',
        'http_coalescing.cpp',
        'http_error.cpp',
        'http_header_parsing.cpp',
        'http_load.cpp',
        'http_noloop.cpp',
        'http_other_loop.cpp',
        'http_reading.cpp',
      ],
    },
  ],
}
