# Integrating a custom build of the Mapbox Maps SDK for macOS into your application

This document explains how to build a development version of the Mapbox Maps SDK for macOS from source and integrate it into your own Cocoa application. This process is for advanced developers who want to get a glimpse of the SDK’s development between releases. To use a production-ready version of the SDK, see the [Mapbox Maps SDK for macOS homepage](https://mapbox.github.io/mapbox-gl-native/macos/).

### Requirements

The Mapbox Maps SDK for macOS requires the macOS 10.10.0 SDK (or above) and Xcode 8.0 (or above). To use this SDK with Xcode 7.3.1, download and use a symbols build from the [releases](https://github.com/mapbox/mapbox-gl-native/releases) page.

### Building the SDK from source

To build the SDK from source:

1. [Install core dependencies](../../INSTALL.md).

1. Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

   ```
   [sudo] gem install jazzy
   ```

1. Run `make xpackage`, which produces a `Mapbox.framework` in the `build/macos/pkg/` folder.

### Installation

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

For further instructions, consult the [macOS SDK documentation](https://mapbox.github.io/mapbox-gl-native/macos/) or run `make xdocument` to generate the documentation. The [Mapbox Maps SDK for iOS](https://www.mapbox.com/ios-sdk/)’s [API documentation](https://www.mapbox.com/ios-sdk/) and [online examples](https://www.mapbox.com/ios-sdk/examples/) apply to the Mapbox Maps SDK for macOS with few differences, mostly around unimplemented features like user location tracking.
