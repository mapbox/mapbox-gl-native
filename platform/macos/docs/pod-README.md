# [Mapbox macOS SDK](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/macos/)

The Mapbox macOS SDK is an open-source framework for embedding interactive map views with scalable, customizable vector maps into Cocoa applications on macOS 10.10.0 and above using Objective-C, Swift, Interface Builder, or AppleScript. It takes stylesheets that conform to the [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-style-spec/), applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://www.mapbox.com/developers/vector-tiles/), and renders them using OpenGL.

<img alt="" src="https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/platform/macos/screenshot.png" width="645">

## Installation

1. Open the project editor, select your application target, then go to the General tab. Drag Mapbox.framework from the `dynamic` folder into the “Embedded Binaries” section. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target, then go to the Info tab. Under the “Custom macOS Target Properties” section, set `MGLMapboxAccessToken` to your access token. You can obtain an access token from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

## Usage

In a storyboard or XIB, add a view to your view controller. (Drag Custom View from the Object library to the View Controller scene on the Interface Builder canvas.) In the Identity inspector, set the view’s custom class to `MGLMapView`. If you need to manipulate the map view programmatically:

1. Switch to the Assistant Editor.
1. Import the `Mapbox` module.
1. Connect the map view to a new outlet in your view controller class. (Control-drag from the map view in Interface Builder to a valid location in your view controller implementation.) The resulting outlet declaration should look something like this:

```objc
// ViewController.m
@import Mapbox;

@interface ViewController : NSViewController

@property (strong) IBOutlet MGLMapView *mapView;

@end
```

```swift
// ViewController.swift
import Mapbox

class ViewController: NSViewController {
    @IBOutlet var mapView: MGLMapView!
}
```

```applescript
-- AppDelegate.applescript
script AppDelegate
    property parent : class "NSObject"
    property theMapView : missing value
end script
```

Full API documentation is included in this package, within the `documentation` folder. The [Mapbox iOS SDK](https://www.mapbox.com/ios-sdk/)’s [API documentation](https://www.mapbox.com/ios-sdk/api/) and [online examples](https://www.mapbox.com/ios-sdk/examples/) apply to the Mapbox macOS SDK with few differences, mostly around unimplemented features like user location tracking.

Mapbox does not officially support the macOS SDK to the same extent as the iOS SDK; however, [bug reports and pull requests](https://github.com/mapbox/mapbox-gl-native/issues/) are certainly welcome.
