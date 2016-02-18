{
  'product_extension': 'app',
  'xcode_settings': {
    'SDKROOT': 'iphoneos',
    'SUPPORTED_PLATFORMS': 'iphonesimulator iphoneos',
    'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
    'TARGETED_DEVICE_FAMILY': '1,2',
    'COPY_PHASE_STRIP': 'NO',
    'CLANG_ENABLE_OBJC_ARC': 'YES',
    'CLANG_ENABLE_MODULES': 'YES',
  },
  'configurations': {
    'Debug': {
      'xcode_settings': {
        'CODE_SIGN_IDENTITY': 'iPhone Developer',
        'COPY_PHASE_STRIP': 'NO',
      },
    },
    'Release': {
      'xcode_settings': {
        'CODE_SIGN_IDENTITY': 'iPhone Distribution',
        'ARCHS': [ "armv7", "armv7s", "arm64", "i386", "x86_64" ],
        'COPY_PHASE_STRIP': 'YES',
      },
    },
  },
}
