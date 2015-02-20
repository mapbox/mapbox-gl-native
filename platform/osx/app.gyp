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
        'app/Icon.icns',
      ],

      'dependencies': [
        '../../mbgl.gyp:bundle_styles',
        '../../mbgl.gyp:core',
        '../../mbgl.gyp:platform-<(platform_lib)',
        '../../mbgl.gyp:http-<(http_lib)',
        '../../mbgl.gyp:asset-<(asset_lib)',
        '../../mbgl.gyp:cache-<(cache_lib)',
      ],

      'sources': [
        'app/main.mm',
        '../darwin/src/settings_nsuserdefaults.mm',
        '../darwin/src/reachability.m',
        '../default/src/glfw_view.cpp',
      ],

      'include_dirs': [
        '../default/src/',
        '../darwin/src/',
      ],

      'variables' : {
        'cflags_cc': [
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
        'OTHER_CPLUSPLUSFLAGS': [ '<@(cflags_cc)' ],
        'OTHER_LDFLAGS': [ '<@(ldflags)' ],
        'SDKROOT': 'macosx',
        'INFOPLIST_FILE': 'app/Info.plist',
        'MACOSX_DEPLOYMENT_TARGET': '10.9',
        'CLANG_ENABLE_OBJC_ARC': 'YES'
      },
    }
  ]
}
