{
  'targets': [
    { 'target_name': 'headless-cgl',
      'product_name': 'mbgl-headless-cgl',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_display.cpp',
      ],

      'include_dirs': [
        '../include',
      ],
    },
  ],
}
