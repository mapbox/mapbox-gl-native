{
  'target_defaults': {
    'default_configuration': 'Release',
    'xcode_settings': {
      'CLANG_CXX_LIBRARY': 'libc++',
      'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
    },
    'cflags_cc': ['-std=c++11'],
    'configurations': {
      'Debug': {
        'cflags': [ '-g', '-O0' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'OTHER_CPLUSPLUSFLAGS': [ '-Wall', '-Wextra', '-pedantic', '-g', '-O0' ]
        }
      },
      'Release': {
        'cflags': [ '-O3' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'OTHER_CPLUSPLUSFLAGS': [ '-Wall', '-Wextra', '-pedantic', '-O3' ]
        }
      }
    }
  }
}
