{
  'targets': [
    { 'target_name': 'npm_install',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'npm install',
          'inputs': [
            '../bin/package.json',
          ],
          'outputs': [
            '../bin/node_modules',
          ],
          'action': ['./scripts/npm_install.sh', '<@(npm)']
        }
      ],
    },
  ]
}
