{
  'target_defaults': {
    'default_configuration': 'Release',
    'xcode_settings': {
      'CLANG_CXX_LIBRARY': 'libc++',
      'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      'GCC_ENABLE_CPP_RTTI':'YES',
      'OTHER_CPLUSPLUSFLAGS': [ '-Wall', '-Wextra' ],
      'GCC_WARN_PEDANTIC': 'YES',
      'GCC_WARN_UNINITIALIZED_AUTOS': 'YES_AGGRESSIVE'
    },
    'cflags_cc': ['-std=c++11'],
    'configurations': {
      'Debug': {
        'cflags': [ '-g', '-O0' ],
        'defines': [ 'DEBUG' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
          'DEAD_CODE_STRIPPING': 'NO',
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'NO',
          'RUN_CLANG_STATIC_ANALYZER':'YES',
          'CLANG_ANALYZER_SECURITY_INSECUREAPI_STRCPY': 'YES',
          'CLANG_ANALYZER_SECURITY_FLOATLOOPCOUNTER': 'YES'
        }
      },
      'Release': {
        'cflags': [ '-O3' ],
        'defines': [ 'NDEBUG' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3',
          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
          'DEAD_CODE_STRIPPING': 'YES',
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES'
        }
      }
    }
  }
}
