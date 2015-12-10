{
  'includes': [
    '../../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'Icon.icns'
      ],

      'dependencies': [
        'mbgl.gyp:core',
        'mbgl.gyp:platform-<(platform_lib)',
        'mbgl.gyp:http-<(http_lib)',
        'mbgl.gyp:asset-<(asset_lib)',
        'mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        './main.mm',
        '../darwin/settings_nsuserdefaults.hpp',
        '../darwin/settings_nsuserdefaults.mm',
        '../darwin/reachability.m',
        '../default/glfw_view.hpp',
        '../default/glfw_view.cpp',
      ],

      'variables' : {
        'cflags_cc': [
          '<@(boost_cflags)',
          '<@(glfw_cflags)',
          '<@(variant_cflags)',
        ],
        'ldflags': [
          '-framework SystemConfiguration', # For NSUserDefaults and Reachability
          '<@(glfw_ldflags)',
        ],
        'libraries': [
          '<@(glfw_static_libs)',
          '<@(boost_libprogram_options_static_libs)'
        ],
      },

      'libraries': [
        '<@(libraries)',
      ],

      'xcode_settings': {
        'SDKROOT': 'macosx',
        'SUPPORTED_PLATFORMS':'macosx',
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        'SDKROOT': 'macosx',
        'INFOPLIST_FILE': '../platform/osx/Info.plist',
        'CLANG_ENABLE_OBJC_ARC': 'YES'
      },
    }
  ]
}
