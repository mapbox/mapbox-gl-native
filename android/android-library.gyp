{
  'includes': [
    '../common.gypi',
    '../config.gypi'
  ],
  # from https://v8.googlecode.com/svn/trunk/build/android.gypi
  # which is from https://src.chromium.org/svn/trunk/src/base/base.gyp
  'variables': {
    # Location of Android NDK.
    'variables': {
      'android_ndk_root%': '<!(/bin/echo -n $ANDROID_NDK_ROOT)',
      'android_toolchain%': '<!(/bin/echo -n $ANDROID_TOOLCHAIN)',
      # This is set when building the Android WebView inside the Android build
      # system, using the 'android' gyp backend.
      'android_webview_build%': 0,
      'android_target_arch': 'arm',
    },
    'conditions': [
      ['android_ndk_root==""', {
        'variables': {
          'android_sysroot': '<(android_toolchain)/sysroot/',
          'android_stlport': '<(android_toolchain)/sources/cxx-stl/stlport/',
        },
        'android_include': '<(android_sysroot)/usr/include',
        'android_lib': '<(android_sysroot)/usr/lib',
        'android_stlport_include': '<(android_stlport)/stlport',
        'android_stlport_libs': '<(android_stlport)/libs',
      }, {
        'variables': {
          'android_sysroot': '<(android_ndk_root)/platforms/android-13/arch-<(android_target_arch)',
          'android_stlport': '<(android_ndk_root)/sources/cxx-stl/stlport/',
        },
        'android_include': '<(android_sysroot)/usr/include',
        'android_lib': '<(android_sysroot)/usr/lib',
        'android_stlport_include': '<(android_stlport)/stlport',
        'android_stlport_libs': '<(android_stlport)/libs',
      }],
    ],
    # Enable to use the system stlport, otherwise statically
    # link the NDK one?
    'use_system_stlport%': '<(android_webview_build)',
    'android_stlport_library': 'stlport_static',
    # Copy it out one scope.
    'android_webview_build%': '<(android_webview_build)',
    'OS': 'android',
  },
  'conditions': [
    [ 'OS == "android"', {
          'targets': [
              {
                  'type': 'static_library',
                  'target_name': 'libllmr',
                  'product_name': 'mylib',
                  'sources': [
                      'android.cpp'
                  ],
                  'dependencies': [
                  ]
              }
          ]
        }
    ]
  ]
}
