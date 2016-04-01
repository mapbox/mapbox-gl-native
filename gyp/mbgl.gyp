{
  'includes': [
    'common.gypi',
    'shaders.gypi',
    'version.gypi',
    'certificates.gypi',
    'standalone.gypi',
    'core.gypi',
    'none.gypi',
  ],

  'targets': [
    { 'target_name': 'loop',
      'product_name': 'mbgl-loop',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        '../platform/default/async_task.cpp',
        '../platform/default/run_loop.cpp',
        '../platform/default/timer.cpp',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'cflags_cc': [
        '<@(libuv_cflags)',
      ],

      'link_settings': {
        'libraries': [
          '<@(libuv_static_libs)',
          '<@(libuv_ldflags)',
        ],
      },

      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [ '<@(libuv_cflags)' ],
          }
        }]
      ],
    },
  ],

  'conditions': [
    ['headless_lib == "cgl" and host == "osx"', { 'includes': [ 'headless-cgl.gypi' ] } ],
    ['headless_lib == "eagl" and host == "ios"', { 'includes': [ 'headless-eagl.gypi' ] } ],
    ['headless_lib == "glx" and host == "linux"', { 'includes': [ 'headless-glx.gypi' ] } ],
    ['platform_lib == "osx" and host == "osx"', { 'includes': [ 'platform-osx.gypi' ] } ],
    ['platform_lib == "ios" and host == "ios"', { 'includes': [ 'platform-ios.gypi' ] } ],
    ['platform_lib == "linux"', { 'includes': [ 'platform-linux.gypi' ] } ],
    ['platform_lib == "android" and host == "android"', { 'includes': [ 'platform-android.gypi' ] } ],
    ['http_lib == "curl"', { 'includes': [ 'http-curl.gypi' ] } ],
    ['http_lib == "nsurl" and (host == "osx" or host == "ios")', { 'includes': [ 'http-nsurl.gypi' ] } ],
    ['http_lib == "android" and host == "android"', { 'includes': [ 'http-android.gypi' ] } ],
    ['asset_lib == "fs"', { 'includes': [ 'asset-fs.gypi' ] } ],
    ['asset_lib == "zip"', { 'includes': [ 'asset-zip.gypi' ] } ],

    ['install_prefix != ""', { 'includes': ['install.gypi' ] } ],
  ],
}
