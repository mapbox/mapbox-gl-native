# [Mapbox macOS SDK](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/macos/)

Put interactive, scalable world maps into your native Cocoa application with the open-source Mapbox macOS SDK.

* Mapbox-curated [map styles](https://www.mapbox.com/maps/) and [vector tiles](https://www.mapbox.com/vector-tiles/) make it easy to get started.
* Customize every aspect of the map’s appearance in code or visually using [Mapbox Studio](https://www.mapbox.com/mapbox-studio/).
* High-performance OpenGL rendering and multitouch gestures keep your users happy.
* A well-designed, fully documented API helps you stay productive.
* Develop across [multiple platforms](https://www.mapbox.com/maps/), including [iOS](https://www.mapbox.com/ios-sdk/), using the same styles and similar APIs.

![](https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/platform/macos/docs/img/screenshot.jpg)

The Mapbox macOS SDK is compatible with macOS 10.10.0 and above for Cocoa applications developed in Objective-C, Swift, Interface Builder, or AppleScript. For hybrid applications, consider [Mapbox GL JS](https://www.mapbox.com/mapbox-gl-js/).

Mapbox macOS SDK releases are [available on GitHub](https://github.com/mapbox/mapbox-gl-native/releases/) – look for the releases that begin with “macos-”. You can also integrate the Mapbox macOS SDK into your application using CocoaPods.

## Installation

1. Open the project editor, select your application target, then go to the General tab. Drag Mapbox.framework into the “Embedded Binaries” section. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target, then go to the Info tab. Under the “Custom macOS Application Target Properties” section, set `MGLMapboxAccessToken` to your access token. You can obtain an access token from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

## Usage

In a storyboard or XIB:

1. Add a view to your view controller or window. (Drag Custom View from the Object library to the View Controller scene on the Interface Builder canvas. In a XIB, drag it instead to the window on the canvas.)
2. In the Identity inspector, set the view’s custom class to `MGLMapView`.
3. MGLMapView needs to be layer-backed:
  * You can make the window layer-backed by selecting the window and checking Full Size Content View in the Attributes inspector. This allows the map view to underlap the title bar and toolbar.
  * Alternatively, if you don’t want the entire window to be layer-backed, you can make just the map view layer-backed by selecting it and checking its entry under the View Effects inspector’s Core Animation Layer section.
4. Add a map feedback item to your Help menu. (Drag Menu Item from the Object library into Main Menu ‣ Help ‣ Menu.) Title it “Improve This Map” or similar, and connect it to the `giveFeedback:` action of First Responder.

If you need to manipulate the map view programmatically:

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

Full API documentation is included in this package, within the `documentation` folder, and [online](https://mapbox.github.io/mapbox-gl-native/macos/). The [Mapbox iOS SDK](https://www.mapbox.com/ios-sdk/)’s [API documentation](https://www.mapbox.com/ios-sdk/api/) and [online examples](https://www.mapbox.com/ios-sdk/examples/) apply to the Mapbox macOS SDK with few differences, mostly around unimplemented features like user location tracking.

Mapbox does not officially support the macOS SDK to the same extent as the iOS SDK; however, [bug reports and pull requests](https://github.com/mapbox/mapbox-gl-native/issues/) are certainly welcome.
