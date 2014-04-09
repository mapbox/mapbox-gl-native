{
  'targets': [
    {
        'target_name': 'iosapp',
        'product_name': 'llmr',
        'type': 'executable',
        'sources': [
            './main.m',
            './MBXAppDelegate.m',
            './MBXSettings.mm',
            './MBXViewController.mm',
            '../common/foundation_request.mm',
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
          'INFOPLIST_FILE': 'Info.plist',
        },
        'dependencies': [
            '../llmr.gyp:llmr'
        ]
    }
  ]
}