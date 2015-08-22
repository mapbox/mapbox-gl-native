Pod::Spec.new do |m|

  m.name    = 'Mapbox-iOS-SDK'
  m.version = '2.0.0-pre.1-symbols'

  m.summary          = 'Open source vector map solution for iOS with full styling capabilities.'
  m.description      = 'Open source OpenGL-based vector map solution for iOS with full styling capabilities and Cocoa bindings.'
  m.homepage         = 'https://www.mapbox.com/ios-sdk/'
  m.license          = 'BSD'
  m.author           = { 'Mapbox' => 'mobile@mapbox.com' }
  m.screenshot       = 'https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/ios/screenshot.png'
  m.social_media_url = 'https://twitter.com/mapbox'
  m.documentation_url = 'https://www.mapbox.com/ios-sdk/'

  m.source = {
    :http => "http://mapbox.s3.amazonaws.com/mapbox-gl-native/ios/builds/mapbox-ios-sdk-#{m.version.to_s}.zip",
    :flatten => true
  }

  m.platform              = :ios
  m.ios.deployment_target = '7.0'

  m.requires_arc = true

  m.preserve_paths = '**'
  m.source_files = 'Headers/*.h', 'MGLDummy.m'
  m.resource_bundle = { 'Mapbox' => 'Mapbox.bundle/*' }
  m.vendored_library = 'libMapbox.a'
  m.module_name = 'Mapbox'

  m.frameworks = 'CoreLocation', 'CoreTelephony', 'GLKit', 'ImageIO', 'MobileCoreServices', 'QuartzCore', 'SystemConfiguration'
  m.libraries = 'c++', 'sqlite3', 'z'
  m.xcconfig = {
    'OTHER_CPLUSPLUSFLAGS' => '-std=gnu++11 -stdlib=libc++',
    'OTHER_LDFLAGS' => '-ObjC',
  }

end
