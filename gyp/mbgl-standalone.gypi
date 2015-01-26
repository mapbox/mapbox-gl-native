{
  'targets': [
    { 'target_name': 'standalone',
      'product_name': 'libmbgl.a',
      'type': 'executable',
      'hard_dependency': 1,

      'dependencies': [
        '<(core_library)',
        '<(platform_library)',
        '<(storage_library)',
        '<(headless_library)',
      ],
    },
  ],
}