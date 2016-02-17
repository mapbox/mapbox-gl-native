{
  'targets': [
    { 'target_name': 'headless-eagl',
      'product_name': 'mbgl-headless-eagl',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/darwin/src/headless_view_eagl.mm',
        '../platform/default/headless_display.cpp',
      ],

      'include_dirs': [
        '../include',
      ],
    },
  ],
}
