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
        'cflags': [ '-g', '-O0', '-I<(boost_root)/include' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'OTHER_CPLUSPLUSFLAGS': [ '-g', '-I<(boost_root)/include' ]
        }
      },
      'Release': {
        'cflags': [ '-O3', '-I<(boost_root)/include' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'OTHER_CPLUSPLUSFLAGS': [ '-I<(boost_root)/include' ]
        }
      }
    }
  }
}
