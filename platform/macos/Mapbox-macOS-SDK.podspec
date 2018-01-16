Pod::Spec.new do |m|

  version = '0.6.1'

  m.name    = 'Mapbox-macOS-SDK'
  m.version = version

  m.summary           = 'Open-source, interactive, fully customizable vector maps.'
  m.description       = 'Interactive, fully customizable vector maps with tight platform integration and high-performance OpenGL rendering.'
  m.homepage          = 'https://mapbox.github.io/mapbox-gl-native/macos/'
  m.license           = { :type => 'BSD', :file => 'LICENSE.md' }
  m.author            = { 'Mapbox' => 'mobile@mapbox.com' }
  m.screenshot        = "https://mapbox.github.io/mapbox-gl-native/macos/#{version}/img/screenshot.jpg"
  m.social_media_url  = 'https://twitter.com/mapbox'
  m.documentation_url = 'https://mapbox.github.io/mapbox-gl-native/macos/'

  m.source = {
    :http => "https://github.com/mapbox/mapbox-gl-native/releases/download/macos-v#{version}/mapbox-macos-sdk-#{m.version.to_s}.zip",
    :flatten => true
  }

  m.platform              = :osx
  m.osx.deployment_target = '10.10'

  m.requires_arc = true

  m.vendored_frameworks = 'Mapbox.framework'
  m.module_name = 'Mapbox'

end
