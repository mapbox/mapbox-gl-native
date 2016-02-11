{
  'includes': [
    '../platform/linux/mapboxgl-app.gypi',
  ],

  'conditions': [
    ['test', { 'includes': [ '../test/test.gypi' ] } ],
    ['offline', { 'includes': [ '../bin/offline.gypi' ] } ],
    ['render', { 'includes': [ '../bin/render.gypi' ] } ],
  ],
}
