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
            "./MBXAppDelegate.h",
            "./MBXAppDelegate.m",
            "./MBXViewController.h",
            "./MBXViewController.mm",
            "./MVKMapKit/MVKMapKit/MVKMapView.h",
            "./MVKMapKit/MVKMapKit/MVKMapView.mm",
            "../common/settings_nsuserdefaults.hpp",
            "../common/settings_nsuserdefaults.mm",
            "../common/foundation_request.h",
            "../common/foundation_request.mm",
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
            '$(SDKROOT)/System/Library/Frameworks/CoreLocation.framework',
            '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework'
          ],
        },
        'xcode_settings': {
          'SDKROOT': 'iphoneos',
          'SUPPORTED_PLATFORMS':['iphonesimulator','iphoneos'],
          'ARCHS': [ "armv7", "armv7s", "arm64", "i386" ],
          'INFOPLIST_FILE': 'Info.plist',
          'CLANG_CXX_LIBRARY': 'libc++',
          'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
          'IPHONEOS_DEPLOYMENT_TARGET':'7.0',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CLANG_ENABLE_OBJC_ARC': 'YES'
        },
        'configurations': {
          'Debug': {
            'xcode_settings': {
              'CODE_SIGN_IDENTITY': 'iPhone Developer',
            }
          },
          'Release': {
            'xcode_settings': {
              'CODE_SIGN_IDENTITY': 'iPhone Distribution',
            }
          }
        },
        "dependencies": [
            "../llmr.gyp:llmr-ios",
            "MVKMapKit/MVKMapKit.gyp:Resources"
        ]
    }
  ]
}