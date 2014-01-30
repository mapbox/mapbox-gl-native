{
  'target_defaults': {
    'default_configuration': 'Release',
    'xcode_settings': {
      'OTHER_CPLUSPLUSFLAGS':['-stdlib=libc++'],
      'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
      'MACOSX_DEPLOYMENT_TARGET':'10.8'
    },
    'conditions': [
      ['OS == "mac"', {
        'defines': [ 'DARWIN' ]
      }, {
        'defines': [ 'LINUX' ]
      }],
      ['OS == "mac"', {
        'xcode_settings': {
          'ARCHS': [ 'x86_64' ]
        },
      }]
    ],
    'configurations': {
      'Debug': {
        'cflags': [ '-g', '-O0' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'OTHER_CFLAGS': [ '-g', '-O0' ]
        }
      },
      'Release': {
        'cflags': [ '-O3' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'OTHER_CFLAGS': [ '-O3' ]
        }
      }
    }
  }
}