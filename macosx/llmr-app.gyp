{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
      'target_name': 'osxapp',
      'product_name': 'llmr',
      'type': 'executable',
      'sources': [
        './main.mm',
        './settings.mm',
        './settings.hpp',
        '../common/glfw_view.hpp',
        '../common/glfw_view.cpp',
        '../common/foundation_request.h',
        '../common/foundation_request.mm',
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
        '../llmr.gyp:llmr-x86'
      ]
    }
  ]
}