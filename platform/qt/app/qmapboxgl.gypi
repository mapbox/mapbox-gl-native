{
  'targets': [
    {
      'target_name': 'qt-app',
      'product_name': 'qmapboxgl',
      'type': 'executable',

      'includes': [
        '../qt.gypi',
      ],

      'dependencies': [
        'qt-lib',
      ],

      'sources': [
        'main.cpp',
        'mapwindow.cpp',
        'mapwindow.hpp',
      ],

      'include_dirs': [
        '../include',
      ],

      'variables': {
        'cflags': [
          '<@(opengl_cflags)',
          '<@(qt_core_cflags)',
          '<@(qt_gui_cflags)',
          '<@(qt_opengl_cflags)',
          '-fPIC',
        ],
        'ldflags': [
          '<@(opengl_ldflags)',
          '<@(qt_core_ldflags)',
          '<@(qt_gui_ldflags)',
          '<@(qt_opengl_ldflags)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags)' ],
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          },
        }, {
          'cflags_cc': [ '<@(cflags)' ],
          'libraries': [ '<@(ldflags)' ],
        }],
      ],
    },
  ],
}
