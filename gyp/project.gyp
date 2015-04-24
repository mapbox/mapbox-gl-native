{
  'conditions': [
    ['host == "osx"', {
      'includes': [
        '../macosx/mapboxgl-app.gypi',
        '../linux/mapboxgl-app.gypi',
        '../test/test.gypi',
        '../bin/render.gypi',
      ]
    }],
    ['host == "linux"', {
      'includes': [
        '../linux/mapboxgl-app.gypi',
        '../test/test.gypi',
        '../bin/render.gypi',
      ]
    }],
  ],
}
