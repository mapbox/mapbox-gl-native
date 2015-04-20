{
  'targets': [
    { 'target_name': 'copy_certificate_bundle',
      'type': 'none',
      'hard_dependency': 1,
      'copies': [{
        'files': [ '../common/api_mapbox_com.der', '../common/ca-bundle.crt' ],
        'destination': '<(PRODUCT_DIR)'
      }],
    },
  ]
}
