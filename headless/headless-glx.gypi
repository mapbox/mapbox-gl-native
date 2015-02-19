{
  'targets': [
    { 'target_name': 'headless-glx',
      'product_name': 'mbgl-headless-glx',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        'mbgl/headless_view.cpp',
        'mbgl/headless_display.cpp',
      ],

      'include_dirs': [
        '../include',
        '.',
      ],
    },
  ],
}
