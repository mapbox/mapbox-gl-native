{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'Icon.icns',
      ],

      'dependencies': [
        '../mbgl.gyp:bundle_styles',
        '../mbgl.gyp:<(core_library)',
        '../mbgl.gyp:<(platform_library)',
        '../mbgl.gyp:<(storage_library)',
      ],

      'sources': [
        './main.mm',
        '../platform/darwin/settings_nsuserdefaults.hpp',
        '../platform/darwin/settings_nsuserdefaults.mm',
        '../platform/darwin/reachability.m',
        '../platform/default/glfw_view.hpp',
        '../platform/default/glfw_view.cpp',
      ],

      'variables' : {
        'cxxflags': [
          '<@(glfw3_cflags)',
        ],
        'ldflags': [
          '-framework SystemConfiguration', # For NSUserDefaults and Reachability
          '<@(glfw3_ldflags)',
        ],
        'libraries': [
          '<@(glfw3_static_libs)',
        ],
      },

      'libraries': [
        '<@(libraries)',
      ],

      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cxxflags)' ],
        'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        'SDKROOT': 'macosx',
        'INFOPLIST_FILE': 'Info.plist',
        'MACOSX_DEPLOYMENT_TARGET': '10.9',
        'CLANG_ENABLE_OBJC_ARC': 'YES'
      },
    }
  ]
}
