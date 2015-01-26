{
  'includes': [
    './gyp/common.gypi',
    './gyp/shaders.gypi',
    './gyp/version.gypi',
    './gyp/styles.gypi',
    './gyp/certificates.gypi',
    './gyp/mbgl-core.gypi',
    './gyp/mbgl-headless.gypi',
    './gyp/mbgl-standalone.gypi',
  ],

  'conditions': [
    ['platform == "osx"', {
      'includes': [
        './gyp/mbgl-osx.gypi',
        './gyp/mbgl-storage-cocoa.gypi',
      ],
    }],
    ['platform == "ios"', {
      'includes': [
        './gyp/mbgl-ios.gypi',
        './gyp/mbgl-storage-cocoa.gypi',
      ],
    }],
    ['platform == "linux"', {
      'includes': [
        './gyp/mbgl-linux.gypi',
        './gyp/mbgl-storage-curl.gypi',
      ],
    }],
    ['platform == "android"', {
      'includes': [
        './gyp/mbgl-android.gypi'
      ],
    }],
  ],
}
