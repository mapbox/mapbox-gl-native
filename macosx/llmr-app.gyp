{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
        "target_name": "osxapp",
        "product_name": "llmr",
        "type": "executable",
        "sources": [
            "./main.mm",
            "./settings.mm",
            "./settings.hpp"
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
          'ARCHS': [ "x86_64" ],
          'SDKROOT': 'macosx',
          'SUPPORTED_PLATFORMS':'macosx',
          'OTHER_CPLUSPLUSFLAGS':[
            '<@(glfw3_cflags)'
          ],
          'OTHER_LDFLAGS': [
            '-stdlib=libc++',
            '<@(glfw3_libraries)',
          ],
          'SDKROOT': 'macosx',
          'INFOPLIST_FILE': 'Info.plist',
          'CLANG_CXX_LIBRARY': 'libc++',
          'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
          'MACOSX_DEPLOYMENT_TARGET':'10.8',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CLANG_ENABLE_OBJC_ARC': 'YES'
        },
        "dependencies": [
            "../llmr.gyp:llmr-osx"
        ]
    }
  ]
}