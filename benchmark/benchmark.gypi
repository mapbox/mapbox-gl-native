{
  'targets': [
    {
      'target_name': 'benchmark-lib',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,
      'dependencies': [
        'core',
      ],

      'include_dirs': [
        '../include',
        '../src',
        '../platform/default',
        'include',
        'src',
      ],

      'sources': [
        'parse/filter.cpp',


        'src/mbgl/benchmark/benchmark.cpp'
      ],

      'variables': {
        'cflags_cc': [
          '<@(benchmark_cflags)',
          '<@(rapidjson_cflags)',
        ],
        'ldflags': [
          '<@(benchmark_ldflags)',
        ],
        'libraries': [
          '<@(benchmark_static_libs)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
          },
        }, {
         'cflags_cc': [ '<@(cflags_cc)' ],
        }],
      ],
      'link_settings': {
        'conditions': [
          ['OS == "mac"', {
            'libraries': [ '<@(libraries)' ],
            'xcode_settings': { 'OTHER_LDFLAGS': [ '<@(ldflags)' ] }
          }, {
            'libraries': [ '<@(libraries)', '<@(ldflags)' ],
          }]
        ],
      },

      'direct_dependent_settings': {
        'include_dirs': [
          'include',
        ],

        # Force the linker to include all the objects from the lib-benchmark archive. Otherwise they'd
        # be discarded because there are no undefined symbols to pull them in, and the resulting
        # executable would run zero tests.

        'conditions': [
          ['OS == "mac"', {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-Wl,-force_load,<(PRODUCT_DIR)/libbenchmark-lib.a',
              ],
            }
          }, {
            'link_settings': {
              'ldflags': [
                '-Wl,-whole-archive <(PRODUCT_DIR)/libbenchmark-lib.a -Wl,-no-whole-archive',
              ],
            },
          }],
        ],
      },
    },
  ]
}
