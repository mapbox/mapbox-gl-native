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
    ['headless_lib == "cgl" and host == "osx"', { 'includes': [ './gyp/headless-cgl.gypi' ] } ],
    ['headless_lib == "glx" and host == "linux"', { 'includes': [ './gyp/headless-glx.gypi' ] } ],
    ['platform_lib == "osx" and host == "osx"', { 'includes': [ './gyp/platform-osx.gypi' ] } ],
    ['platform_lib == "ios" and host == "ios"', { 'includes': [ './gyp/platform-ios.gypi' ] } ],
    ['platform_lib == "linux"', { 'includes': [ './gyp/platform-linux.gypi' ] } ],
    ['platform_lib == "android" and host == "android"', { 'includes': [ './gyp/platform-android.gypi' ] } ],
    ['http_lib == "curl"', { 'includes': [ './gyp/http-curl.gypi' ] } ],
    ['http_lib == "nsurl" and (host == "osx" or host == "ios")', { 'includes': [ './gyp/http-nsurl.gypi' ] } ],
    ['http_lib == "android" and host == "android"', { 'includes': [ './gyp/http-android.gypi' ] } ],
    ['asset_lib == "fs"', { 'includes': [ './gyp/asset-fs.gypi' ] } ],
    ['asset_lib == "zip"', { 'includes': [ './gyp/asset-zip.gypi' ] } ],
    ['cache_lib == "sqlite"', { 'includes': [ './gyp/cache-sqlite.gypi' ] } ],

    ['install_prefix != ""', { 'includes': ['./gyp/install.gypi' ] } ],
  ],
}
