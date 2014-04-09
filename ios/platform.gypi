{
  'variables': {
    'shader_type': 'gles2',
  },
  'target_defaults': {
    'xcode_settings': {
      'ARCHS': ['armv7', 'armv7s', 'arm64', 'i386'],
      'SDKROOT': 'iphoneos',
      'SUPPORTED_PLATFORMS': ['iphonesimulator', 'iphoneos'],
      'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
      'PUBLIC_HEADERS_FOLDER_PATH': 'include',
      # 'OTHER_LDFLAGS': [
      #   '-stdlib=libc++',
      # ],
      # 'CLANG_CXX_LIBRARY': 'libc++',
      # 'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
      'TARGETED_DEVICE_FAMILY': '1,2',
      'CODE_SIGN_IDENTITY': 'iPhone Developer',
      # 'GCC_INPUT_FILETYPE': 'sourcecode.cpp.cpp',
      # 'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      # 'CLANG_ENABLE_OBJC_ARC': 'YES',
    },
    'configurations': {
      'Debug': {
        'xcode_settings': {
          'CODE_SIGN_IDENTITY': 'iPhone Developer',
        },
      },
      'Release': {
        'xcode_settings': {
          'CODE_SIGN_IDENTITY': 'iPhone Distribution',
        },
      },
    },
  },
}
