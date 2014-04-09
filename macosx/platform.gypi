{
  'variables': {
    'shader_type': 'gl',
  },
  'target_defaults': {
    'xcode_settings': {
      'ARCHS': [ "x86_64" ],
      'SDKROOT': 'macosx',
      'SUPPORTED_PLATFORMS': ['macosx'],
      'MACOSX_DEPLOYMENT_TARGET': '10.9',
      'PUBLIC_HEADERS_FOLDER_PATH': 'include',
    },
  },
}
