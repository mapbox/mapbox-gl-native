{
  'includes': [
    './common.gypi',
    './gyp/shaders.gypi',
    './gyp/npm_install.gypi',
    './gyp/styles.gypi',
    './gyp/fixtures.gypi',
    './gyp/certificates.gypi',
    './gyp/library.gypi',
  ],
  'conditions': [
    ['platform == "osx"', { 'includes': ['./gyp/platform-osx.gypi'] }],
    ['platform == "ios"', { 'includes': ['./gyp/platform-ios.gypi'] }],
    ['platform == "linux"', { 'includes': ['./gyp/platform-linux.gypi'] }],
  ],
}
