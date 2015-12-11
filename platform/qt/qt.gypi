{
  'rules': [
    {
      'rule_name': 'MOC files',
      'extension': 'hpp',
      'process_outputs_as_sources': 1,
      'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cpp' ],
      'action': [ '<(qt_moc)', '<(RULE_INPUT_PATH)', '-o', '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cpp' ],
      'message': 'Generating MOC <(RULE_INPUT_ROOT).cpp',
    },
    {
      'rule_name': 'QRC files',
      'extension': 'qrc',
      'process_outputs_as_sources': 1,
      'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/qrc_<(RULE_INPUT_ROOT).cpp' ],
      'action': [ '<(qt_rcc)', '<(RULE_INPUT_PATH)', '-name', '<(RULE_INPUT_ROOT)', '-o', '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/qrc_<(RULE_INPUT_ROOT).cpp' ],
      'message': 'Generating QRC <(RULE_INPUT_ROOT).cpp',
    },
  ],
}
