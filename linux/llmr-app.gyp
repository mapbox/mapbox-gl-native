{
  'targets': [
    {
      'target_name': 'linuxapp',
      'product_name': 'llmr',
      'type': 'executable',
      'sources': [
        './main.cpp',
        './settings.cpp',
        './settings.hpp',
        './request.cpp',
        './request.hpp'
      ],
      'conditions': [
        ['OS == "mac"', {
          'product_extension': 'app',
          'mac_bundle': 1,
          'mac_bundle_resources': [
            '../macosx/Icon.icns',
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
              '<@(glfw3_cflags)',
              '<@(curl_cflags)',
              '-I<(boost_root)/include',
            ],
            'OTHER_LDFLAGS': [
              '<@(glfw3_libraries)',
              '<@(curl_libraries)',
            ],
            'INFOPLIST_FILE': '../macosx/Info.plist',
          },
        }, {
          'link_settings': {
            'libraries': [
              '<@(glfw3_libraries)',
              '<@(curl_libraries)',
            ],
          },
          'cflags': [
            '<@(glfw3_cflags)',
            '<@(curl_cflags)',
            '-I<(boost_root)/include',
          ],
        }]
      ],
      'dependencies': [
        '../llmr.gyp:llmr',
      ]
    }
  ]
}
