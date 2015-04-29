{
  'targets': [
    { 'target_name': 'headless-glx',
      'product_name': 'mbgl-headless-glx',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_display.cpp',
      ],

      'include_dirs': [
        '../include',
      ],

      'cflags_cc': [ '<@(opengl_cflags)' ],

      'link_settings': {
        'libraries': [ '<@(opengl_ldflags)' ],
      },
    },
  ],
}
