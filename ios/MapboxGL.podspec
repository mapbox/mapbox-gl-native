Pod::Spec.new do |m|

  m.name    = 'MapboxGL'
  m.version = '0.1.0'

  m.summary          = 'Open source vector map solution for iOS with full styling capabilities.'
  m.description      = 'Open source OpenGL-based vector map solution for iOS with full styling capabilities and Cocoa bindings.'
  m.homepage         = 'https://www.mapbox.com/blog/mapbox-gl/'
  m.license          = 'BSD'
  m.author           = { 'Mapbox' => 'mobile@mapbox.com' }
  m.screenshot       = 'https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/ios/screenshot.png'
  m.social_media_url = 'https://twitter.com/mapbox'

  m.source = { :http => "http://mapbox.s3.amazonaws.com/mapbox-gl-native/ios/mapbox-gl-ios-#{m.version.to_s}.zip" }

  m.platform              = :ios
  m.ios.deployment_target = '7.0'

  m.source_files = 'Headers/*.h'

  m.requires_arc = true

  m.resource_bundle = { 'MapboxGL' => 'MapboxGL.bundle/*' }

  m.frameworks = 'CoreLocation', 'CoreTelephony', 'GLKit', 'ImageIO', 'MobileCoreServices', 'QuartzCore', 'SystemConfiguration'

  m.libraries = 'c++', 'sqlite3', 'z'

  m.vendored_library = 'libMapboxGL.a'

  m.xcconfig = { 'OTHER_CPLUSPLUSFLAGS' => '-std=gnu++11 -stdlib=libc++' }

end
