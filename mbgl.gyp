{
  'includes': [
    './gyp/common.gypi',
    './gyp/shaders.gypi',
    './gyp/version.gypi',
    './gyp/styles.gypi',
    './gyp/certificates.gypi',
    './gyp/standalone.gypi',
    './gyp/core.gypi',
    './gyp/none.gypi',
  ],
  'conditions': [
    ['headless_lib == "cgl" and host == "osx"', { 'includes': [ './headless/headless-cgl.gypi' ] } ],
    ['headless_lib == "glx" and host == "linux"', { 'includes': [ './headless/headless-glx.gypi' ] } ],
    ['platform_lib == "osx" and host == "osx"', { 'includes': [ './platform/osx/platform.gypi' ] } ],
    ['platform_lib == "ios" and host == "ios"', { 'includes': [ './platform/ios/platform.gypi' ] } ],
    ['platform_lib == "linux"', { 'includes': [ './platform/linux/platform.gypi' ] } ],
    ['platform_lib == "android" and host == "android"', { 'includes': [ './platform/android/platform.gypi' ] } ],
    ['http_lib == "curl"', { 'includes': [ './gyp/http-curl.gypi' ] } ],
    ['http_lib == "nsurl" and (host == "osx" or host == "ios")', { 'includes': [ './gyp/http-nsurl.gypi' ] } ],
    ['asset_lib == "fs"', { 'includes': [ './gyp/asset-fs.gypi' ] } ],
    ['asset_lib == "zip"', { 'includes': [ './gyp/asset-zip.gypi' ] } ],
    ['cache_lib == "sqlite"', { 'includes': [ './gyp/cache-sqlite.gypi' ] } ],

    ['install_prefix != ""', { 'includes': ['./gyp/install.gypi' ] } ],
  ],
}
