# Integrating the Mapbox OS X SDK into your application

This document explains how to build the Mapbox OS X SDK and integrate it into your own Cocoa application.

### Requirements

The Mapbox OS X SDK requires the OS X 10.10.0 SDK or above.

### Building the SDK

1. [Install core dependencies](../../INSTALL.md).
1. Run `make xpackage`, which produces a `Mapbox.framework` in the `gyp/build/Release/` folder.

### Installation

1. Open the project editor, select your application target, then go to the General tab. Drag Mapbox.framework from the `build/osx/Build/Products/Release/` directory into the “Embedded Binaries” section. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target, then go to the Info tab. Under the “Custom OS X Application Target Properties” section, set `MGLMapboxAccessToken` to your access token. You can obtain an access token from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

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

The [Mapbox iOS SDK’s API documentation](https://www.mapbox.com/ios-sdk/api/) applies to the Mapbox OS X SDK with few differences, mostly around unimplemented features like user location tracking.
