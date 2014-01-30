{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
        "target_name": "test_app",
        "product_name": "llmr",
        "type": "executable",
        "sources": [
            "./main.mm",
            "./settings.mm"
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
            '-stdlib=libc++'
          ],
          'OTHER_CFLAGS':[
            '<@(glfw3_cflags)'
          ],
          'SDKROOT': 'macosx',
          'INFOPLIST_FILE': 'Info.plist',
        },
        "dependencies": [
            "../llmr.gyp:llmr"
        ]
    }
  ]
}