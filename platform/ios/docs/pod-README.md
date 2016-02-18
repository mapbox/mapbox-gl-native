# [Mapbox iOS SDK](https://www.mapbox.com/ios-sdk/)

The Mapbox iOS SDK is an open-source framework for embedding interactive map views with scalable, customizable vector maps into Cocoa Touch applications on iOS 7.0 and above using Objective-C, Swift, or Interface Builder. It takes stylesheets that conform to the [Mapbox GL Style Specification](https://www.mapbox.com/mapbox-gl-style-spec/), applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://www.mapbox.com/developers/vector-tiles/), and renders them using OpenGL.

For more information, check out the [Mapbox iOS SDK homepage](https://www.mapbox.com/ios-sdk/) and the [full changelog](https://github.com/mapbox/mapbox-gl-native/blob/master/CHANGELOG.md) online.

[![](https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/ios/screenshot.png)]()

## Installation

The Mapbox iOS SDK may be installed as either a dynamic framework or a static framework. (To reduce the download size, the static framework is omitted from some distributions; you may need to download the full package from the [release page](https://github.com/mapbox/mapbox-gl-native/releases/).)

{{DYNAMIC}}

### Dynamic framework

This is the recommended workflow for manually integrating the SDK into an application targeting iOS 8 and above:

1. Open the project editor and select your application target. Drag `Mapbox.framework` from the `dynamic` folder into the “Embedded Binaries” section of the General tab. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. In the Build Phases tab, click the + button at the top and select “New Run Script Phase”. Enter the following code into the script text field:

```bash
bash "${BUILT_PRODUCTS_DIR}/${FRAMEWORKS_FOLDER_PATH}/Mapbox.framework/strip-frameworks.sh"
```

(The last step, courtesy of [Realm](https://github.com/realm/realm-cocoa/), is required for working around an [iOS App Store bug](http://www.openradar.me/radar?id=6409498411401216) when archiving universal binaries.)

{{/DYNAMIC}}
{{STATIC}}

### Static framework

If your application targets iOS 7.x, you’ll need to install the static framework instead:

1. Open the project editor and select your application target. Drag `Mapbox.framework` from the `static` folder into the “Embedded Binaries” section of the General tab. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. Add the following Cocoa Touch frameworks and libraries to the “Linked Frameworks and Libraries” section:

   - `GLKit.framework`
   - `ImageIO.framework`
   - `MobileCoreServices.framework`
   - `QuartzCore.framework`
   - `SystemConfiguration.framework`
   - `libc++.dylib`
   - `libsqlite3.dylib`
   - `libz.dylib`

1. In the Build Settings tab, add `-ObjC` to the “Other Linker Flags” (`OTHER_LDFLAGS`) build setting.

{{/STATIC}}

## Configuration

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

1. In order to show the user’s position on the map, you must first ask for their permission. In iOS 8 and above, this is accomplished by creating and setting the `NSLocationAlwaysUsageDescription` and/or `NSLocationWhenInUseUsageDescription` key in the `Info.plist` file.

1. _(Optional)_ Mapbox Telemetry is a <a href="https://www.mapbox.com/telemetry/">powerful location analytics platform</a> included in this SDK. By default, anonymized location and usage data is sent to Mapbox whenever the host application causes it to be gathered. This SDK provides users with a way to individually opt out of Mapbox Telemetry. You can also add this opt-out setting to your application’s Settings screen, using the provided `Settings.bundle`.

## Usage

In a XIB or storyboard, add a Custom View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the Mapbox module in your view controller’s implementation file:

```objc
// ViewController.m
@import Mapbox;
```

```swift
// ViewController.swift
import Mapbox
```

Then switch to the Assistant Editor and create an outlet to the map view. (Control-drag from the map view in Interface Builder to a valid location in your view controller implementation.)

Full API documentation is included in this package, within the `documentation` folder. For more details, read “[First steps with the Mapbox iOS SDK](https://www.mapbox.com/help/first-steps-ios-sdk/)” and consult the [online examples](https://www.mapbox.com/ios-sdk/examples/).

If you have any questions, please [contact our support team](https://www.mapbox.com/contact/). We welcome your [bug reports and feature requests](https://github.com/mapbox/mapbox-gl-native/issues/).
