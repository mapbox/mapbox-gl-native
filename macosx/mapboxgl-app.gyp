{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
      'target_name': 'osxapp',
      'product_name': 'Mapbox GL',
      'type': 'executable',
      'sources': [
        './main.mm',
        '../common/settings_nsuserdefaults.hpp',
        '../common/settings_nsuserdefaults.mm',
        '../common/glfw_view.hpp',
        '../common/glfw_view.cpp',
        '../common/foundation_request.h',
        '../common/foundation_request.mm',
        '../common/nslog_log.hpp',
        '../common/nslog_log.mm',
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
          '<@(glfw3_libraries)',
        ],
        'SDKROOT': 'macosx',
        'INFOPLIST_FILE': 'Info.plist',
        'MACOSX_DEPLOYMENT_TARGET':'10.9',
        'CLANG_ENABLE_OBJC_ARC': 'YES'
      },
      'dependencies': [
        '../mapboxgl.gyp:bundle_styles',
        '../mapboxgl.gyp:mapboxgl',
      ]
    }
  ]
}