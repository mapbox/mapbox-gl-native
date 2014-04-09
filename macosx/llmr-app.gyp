{
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
        '../common/foundation_request.mm',
      ],
      'product_extension': 'app',
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'Icon.icns',
      ],
      'link_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
          '$(SDKROOT)/System/Library/Frameworks/IOKit.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework',
        ],
      },
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS':[
          '<@(glfw3_cflags)'
        ],
        'OTHER_LDFLAGS': [
          '<@(glfw3_libraries)',
        ],
        'INFOPLIST_FILE': 'Info.plist',
      },
      'dependencies': [
        '../llmr.gyp:llmr'
      ]
    }
  ]
}