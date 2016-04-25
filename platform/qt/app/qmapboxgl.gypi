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
          '<@(qt_cflags)',
          '-Wno-error',  # TODO: eliminate
          '-fPIC',
        ],
        'ldflags': [
          '<@(opengl_ldflags)',
          '<@(qt_ldflags)'
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
