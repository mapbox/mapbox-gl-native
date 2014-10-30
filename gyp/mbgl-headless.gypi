{
  'targets': [
    { 'target_name': 'mbgl-headless',
      'product_name': 'mbgl-headless',
      'type': 'static_library',
      'standalone_static_library': 1,
      'include_dirs': [
        '../include',
      ],
      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_display.cpp',
      ],
    },
  ],
}
