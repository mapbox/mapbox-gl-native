  m.ios.deployment_target = '8.0'

  m.requires_arc = true

  m.preserve_paths = '**'
  m.source_files = 'Headers/*.h', 'MGLDummy.m'
  m.resource_bundle = { 'Mapbox' => 'Mapbox.bundle/*' }
  m.vendored_frameworks = 'Mapbox.framework'
  m.module_name = 'Mapbox'
  m.xcconfig = { 'OTHER_LDFLAGS' => '-ObjC' }

end
