{
  'targets': [
    {
      'target_name': 'qt-qml-app',
      'product_name': 'qquickmapboxgl',
      'type': 'executable',

      'includes': [
        '../qt.gypi',
      ],

      'dependencies': [
        'qt-lib',
      ],

      'sources': [
        'main.cpp',
        'qml.qrc',
      ],

      'include_dirs': [
        '../include',
      ],

      'variables': {
        'cflags': [
          '<@(opengl_cflags)',
          '<@(qt_core_cflags)',
          '<@(qt_gui_cflags)',
          '<@(qt_positioning_cflags)',
          '<@(qt_qml_cflags)',
          '<@(qt_quick_cflags)',
          '-fPIC',
        ],
        'ldflags': [
          '<@(opengl_ldflags)',
          '<@(qt_core_ldflags)',
          '<@(qt_gui_ldflags)',
          '<@(qt_positioning_ldflags)',
          '<@(qt_qml_ldflags)',
          '<@(qt_quick_ldflags)',
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
