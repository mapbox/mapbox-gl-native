{
    'includes': [
      '../../gyp/common.gypi',
    ],
    'targets': [{
        'target_name': 'gtest',
        'type': 'static_library',
        'sources': [
            'gtest-all.cc'
        ],
        'direct_dependent_settings': {
            'include_dirs': [
              '.',
            ],
        },
    }]
}