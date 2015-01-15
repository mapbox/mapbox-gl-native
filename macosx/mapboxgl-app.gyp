{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'sources': [
        './main.mm',
        '../platform/darwin/settings_nsuserdefaults.hpp',
        '../platform/darwin/settings_nsuserdefaults.mm',
        '../platform/darwin/reachability.m',
        '../platform/default/glfw_view.hpp',
        '../platform/default/glfw_view.cpp',
      ],
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'Icon.icns',
      ],
      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'OTHER_CPLUSPLUSFLAGS': [
          '<@(glfw3_cflags)'
        ],
        'OTHER_LDFLAGS': [
          '<@(glfw3_ldflags)',
          '-framework SystemConfiguration',
        ],
        'SDKROOT': 'macosx',
        'INFOPLIST_FILE': 'Info.plist',
        'MACOSX_DEPLOYMENT_TARGET':'10.9',
        'CLANG_ENABLE_OBJC_ARC': 'YES'
      },
      'variables' : {
        'ldflags': [
          '<@(uv_ldflags)',
          '<@(uv_static_libs)',
          '<@(sqlite3_static_libs)',
          '<@(sqlite3_ldflags)',
          '<@(glfw3_static_libs)',
          '<@(glfw3_ldflags)',
          '<@(zlib_ldflags)',
        ]
      },
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [ '<@(ldflags)' ],
          }
        }, {
          'ldflags': [ '<@(ldflags)' ],
        }]
      ],
      'dependencies': [
        '../mapboxgl.gyp:bundle_styles',
        '../mapboxgl.gyp:mbgl-core',
        '../mapboxgl.gyp:mbgl-osx',
      ]
    }
  ]
}
