{
  'targets': [
    { 'target_name': 'mbgl-headless',
      'product_name': 'mbgl-headless',
      'type': 'static_library',
      'standalone_static_library': 1,
      'variables': {
        'cflags_cc': [
          '<@(uv_cflags)',
        ],
      },
      'include_dirs': [
        '../include',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          }
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
        }]
      ],
      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_display.cpp',
      ],
    },
  ],
}
