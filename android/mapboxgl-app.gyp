{
  'includes': [
    '../common.gypi',
    '../config.gypi',
  ],
  'targets': [
    {
      'target_name': 'androidapp',
      'product_name': 'mapbox-gl',
      'type': 'none',
      'dependencies': [
        '../mapboxgl.gyp:mapboxgl',
      ],
    },
  ],
}
