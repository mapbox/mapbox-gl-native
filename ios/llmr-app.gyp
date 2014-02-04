{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  'targets': [
    {
        "target_name": "iosapp",
        "product_name": "llmr",
        "type": "executable",
        "sources": [
            "./main.m",
            "./MBXAppDelegate.m",
            "./MBXSettings.mm",
            "./MBXViewController.mm"
        ],
        'product_extension': 'app',
        'mac_bundle': 1,
        'mac_bundle_resources': [
          '<!@(find img -type f)'
        ],
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/UIKit.framework',
            '$(SDKROOT)/System/Library/Frameworks/OpenGLES.framework',
            '$(SDKROOT)/System/Library/Frameworks/GLKit.framework',
            '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework'
          ],
        },
        'xcode_settings': {
          'ARCHS': [ "armv7", "armv7s", "arm64" ],
          'SUPPORTED_PLATFORMS': [ 'iphoneos', 'iphonesimulator' ],
          'OTHER_LDFLAGS': [
            '-stdlib=libc++'
          ],
          'SDKROOT': 'iphoneos',
          'INFOPLIST_FILE': 'Info.plist',
          'CLANG_CXX_LIBRARY': 'libc++',
          'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
          'IPHONEOS_DEPLOYMENT_TARGET':'5.0',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'CODE_SIGN_IDENTITY': 'iPhone Developer',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CLANG_ENABLE_OBJC_ARC': 'YES'
        },
        "dependencies": [
            "../llmr.gyp:llmr-ios"
        ]
    }
  ]
}