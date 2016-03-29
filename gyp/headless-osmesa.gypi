{
  'targets': [
    { 'target_name': 'headless-osmesa',
      'product_name': 'mbgl-headless-osmesa',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        '../platform/default/headless_view.cpp',
        '../platform/default/headless_view_osmesa.cpp',
        '../platform/default/headless_display.cpp',
      ],

      'include_dirs': [
        '../include',
        '/usr/local/lib/osmesa/include',
      ],

      'cflags_cc': [ '<@(opengl_cflags)' ],

      'link_settings': {
        'libraries': [ '<@(opengl_ldflags)' ],
      },
    },
  ],
}
