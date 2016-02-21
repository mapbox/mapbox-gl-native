{
  'includes': [
    '../platform/osx/app/mapboxgl-app.gypi',
    '../platform/osx/sdk/framework-osx.gypi',
    '../platform/osx/test/osxtest.gypi',
    '../platform/linux/mapboxgl-app.gypi',
  ],

  'conditions': [
    ['test', { 'includes': [ '../test/test.gypi' ] } ],
    ['offline', { 'includes': [ '../bin/offline.gypi' ] } ],
    ['render', { 'includes': [ '../bin/render.gypi' ] } ],
  ],
}
