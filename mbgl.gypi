{
  'target_defaults': {
    'default_configuration': 'Release',
    'conditions': [
      ['OS=="mac"', {
        'xcode_settings': {
          'CLANG_CXX_LIBRARY': 'libc++',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          'GCC_ENABLE_CPP_RTTI': 'YES',
          'OTHER_CPLUSPLUSFLAGS': [
            '-std=c++14',
            '-Werror',
            '-Wall',
            '-Wextra',
            '-Wshadow',
            '-Wno-variadic-macros',
            '-frtti',
            '-fexceptions',
            '${CFLAGS}',
          ],
          'GCC_WARN_PEDANTIC': 'YES',
          'GCC_WARN_UNINITIALIZED_AUTOS': 'YES_AGGRESSIVE',
          'MACOSX_DEPLOYMENT_TARGET': '10.10',
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'COMBINE_HIDPI_IMAGES': 'YES',
        },
      }, {
        'cflags_cc': [
          '-std=c++14',
          '-Werror',
          '-Wall',
          '-Wextra',
          '-Wshadow',
          '-Wno-variadic-macros',
          '-Wno-error=unused-parameter',
          '-frtti',
          '-fexceptions',
          '${CFLAGS}',
        ],
      }],
      ['OS=="linux"', {
        'cflags_cc': [
          '-Wno-unknown-pragmas', # We are using '#pragma mark', but it is only available on Darwin.
        ],
      }],
    ],
    'target_conditions': [
      ['_type == "static_library"', {
        'conditions': [
          ['OS=="mac"', {
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': [ '-fPIC' ],
              'SKIP_INSTALL': 'YES',
            },
          }, {
            'cflags_cc': [ '-fPIC' ],
          }],
        ],
      }],
    ],
    'configurations': {
      'Debug': {
        'conditions': [
          ['OS=="mac"', {
            'xcode_settings': {
              'DEBUG_INFORMATION_FORMAT': 'dwarf',
              'GCC_OPTIMIZATION_LEVEL': '0',
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
              'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
              'DEAD_CODE_STRIPPING': 'NO',
              'OTHER_CPLUSPLUSFLAGS': [ '-fno-omit-frame-pointer','-fwrapv', '-fstack-protector-all', '-fno-common' ],
              'conditions': [
                ['coverage', {
                  'GCC_INSTRUMENT_PROGRAM_FLOW_ARCS': 'YES',
                  'GCC_GENERATE_TEST_COVERAGE_FILES': 'YES',
                  'OTHER_CPLUSPLUSFLAGS': [ '--coverage' ],
                }],
              ],
            },
          }, {
            'cflags_cc': [ '-g', '-O0', '-fno-omit-frame-pointer','-fwrapv', '-fstack-protector-all', '-fno-common' ],
            'conditions': [
              ['coverage', { 'cflags_cc': [ '--coverage' ] }],
            ],
          }],
        ],
        'defines': [ 'DEBUG' ],
        'target_conditions': [
          ['_type == "executable"', {
            'conditions': [
              ['OS=="mac" and coverage', {
                 'xcode_settings': { 'OTHER_LDFLAGS': [ '--coverage' ] },
              }, {
                'ldflags': [ '--coverage' ],
              }],
            ],
          }],
        ],
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'conditions': [
          ['OS=="mac"', {
            'xcode_settings': {
              'GCC_OPTIMIZATION_LEVEL': '3',
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
              'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
              'DEAD_CODE_STRIPPING': 'NO',
            },
          }, {
            'cflags_cc': [ '-g', '-O3' ],
          }],
        ],
      },
    },
  },
  'configurations': {
    'Debug': {
      'xcode_settings': {
        'ENABLE_TESTABILITY': 'YES',
        'ONLY_ACTIVE_ARCH': 'YES',
      },
    },
  },
  'targets': [
    {
      'target_name': 'core',
      'product_name': 'mbgl-core',
      'type': 'static_library',
      'standalone_static_library': 1,
      'hard_dependency': 1,

      'sources': [
        '<!@(find <(DEPTH)/src -name "*.hpp")',
        '<!@(find <(DEPTH)/src -name "*.cpp")',
        '<!@(find <(DEPTH)/src -name "*.c")',
        '<!@(find <(DEPTH)/src -name "*.h")',
        '<!@(find <(DEPTH)/include -name "*.hpp")',
        '<!@(find <(DEPTH)/include -name "*.h")',
        '<!@(find -H <(DEPTH)/node_modules/mapbox-gl-shaders -name "*.glsl")',
        '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp',
      ],

      'rules': [
        {
          'rule_name': 'Build Shaders',
          'message': 'Building shader',
          'extension': 'glsl',
          'inputs': [ '<(DEPTH)/scripts/build-shaders.py' ],
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/<(RULE_INPUT_ROOT).hpp' ],
          'action': [ '<@(_inputs)', '<(RULE_INPUT_PATH)', '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/shader/<(RULE_INPUT_ROOT).hpp' ],
          'process_outputs_as_sources': 1,
        }
      ],

      'actions': [
        {
          'action_name': 'Build Version Header',
          'inputs': [ '<(DEPTH)/scripts/build-version.py', ],
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/include/mbgl/util/version.hpp', ],
          'action': [ '<@(_inputs)', '<(SHARED_INTERMEDIATE_DIR)' ],
        }
      ],

      'include_dirs': [
        'include',
        'src',
        '<(SHARED_INTERMEDIATE_DIR)/include',
      ],

      'variables': {
        'cflags_cc': [
          '<@(opengl_cflags)',
          '<@(protozero_cflags)',
          '<@(boost_cflags)',
          '<@(geometry_cflags)',
          '<@(geojsonvt_cflags)',
          '<@(rapidjson_cflags)',
          '<@(variant_cflags)',
          '<@(earcut_cflags)',
        ],
        'cflags': [
          '<@(opengl_cflags)',
          '<@(rapidjson_cflags)',
          '-fPIC'
        ],
        'ldflags': [
          '<@(opengl_ldflags)',
        ],
        'libraries': [
          '<@(geojsonvt_static_libs)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
            'OTHER_CFLAGS': [ '<@(cflags)' ],
          },
        }, {
          'cflags_cc': [ '<@(cflags_cc)' ],
          'cflags': [ '<@(cflags)' ],
        }],

        ['headless_lib == "cgl"', {
          'sources': [
            'platform/default/headless_display.cpp',
            'platform/default/headless_view.cpp',
            'platform/darwin/src/headless_view_cgl.cpp',
          ],

          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
            ],
          },
        }],

        ['headless_lib == "eagl"', {
          'sources': [
            'platform/default/headless_display.cpp',
            'platform/default/headless_view.cpp',
            'platform/darwin/src/headless_view_eagl.mm',
          ],
        }],

        ['headless_lib == "glx"', {
          'sources': [
            'platform/default/headless_display.cpp',
            'platform/default/headless_view.cpp',
            'platform/default/headless_view_glx.cpp',
          ],

          'cflags_cc': [ '<@(opengl_cflags)' ],

          'link_settings': {
            'libraries': [ '<@(opengl_ldflags)' ],
          },
        }],

        ['loop_lib == "darwin"', {
          'sources': [
            'platform/darwin/src/async_task.cpp',
            'platform/darwin/src/run_loop.cpp',
            'platform/darwin/src/timer.cpp',
          ],
        }],

        ['loop_lib == "android"', {
          'sources': [
            'platform/android/src/async_task.cpp',
            'platform/android/src/run_loop.cpp',
            'platform/android/src/timer.cpp',
          ],
        }],

        ['loop_lib == "uv"', {
          'sources': [
            'platform/default/async_task.cpp',
            'platform/default/run_loop.cpp',
            'platform/default/timer.cpp',
          ],

          'cflags_cc': [
            '<@(libuv_cflags)',
          ],

          'link_settings': {
            'libraries': [
              '<@(libuv_static_libs)',
              '<@(libuv_ldflags)',
            ],
          },
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
        'cflags_cc': [
          '<@(variant_cflags)',
          '<@(geometry_cflags)',
        ],
        'xcode_settings': {
          'OTHER_CPLUSPLUSFLAGS': [
            '<@(variant_cflags)',
            '<@(geometry_cflags)',
          ],
        },
      },
    },
    {
      'target_name': 'copy_certificate_bundle',
      'type': 'none',
      'hard_dependency': 1,
      'copies': [{
        'files': [ 'common/ca-bundle.crt' ],
        'destination': '<(PRODUCT_DIR)'
      }],
    },
  ],
}
