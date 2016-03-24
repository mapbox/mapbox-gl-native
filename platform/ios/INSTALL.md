# Integrating the Mapbox iOS SDK into your application

This document explains how to build a development version of Mapbox iOS SDK for the purpose of incorporating it into your own Cocoa Touch application.

### Requirements

The Mapbox iOS SDK and iosapp demo application build against the iOS 7.0 SDK. They are intended to run on iOS 7.0 and above on the following devices and their simulators:

* iPhone 4S and above (5, 5c, 5s, 6, 6 Plus)
* iPad 2 and above (3, 4, Mini, Air, Mini 2, Air 2)
* iPod touch 5th generation and above

### Build

1. [Install core dependencies](../../INSTALL.md).

1. Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

   ```
   [sudo] gem install jazzy
   ```

1. Run `make ipackage`. The packaging script will produce a `build/ios/pkg/` folder containing:
  - a `dynamic` folder containing a dynamically-linked fat framework with debug symbols for devices and the iOS Simulator
  - a `static` folder containing a statically-linked framework with debug symbols for devices and the iOS Simulator
  - a `documentation` folder with HTML API documentation
  - an example `Settings.bundle` containing an optional Mapbox Telemetry opt-out setting

### Install

There are a few ways to install the Mapbox iOS SDK:

#### CocoaPods

Currently, until [#1437](https://github.com/mapbox/mapbox-gl-native/issues/1437) is completed, to install a _development version_ of Mapbox GL using CocoaPods you will need to build it from source manually per above.

1. Zip up the build product.

    ```
    cd build/ios/pkg/
    ZIP=mapbox-ios-sdk.zip
    rm -f ../${ZIP}
    zip -r ../${ZIP} *
    ```

1. Customize [`Mapbox-iOS-SDK.podspec`](../ios/Mapbox-iOS-SDK.podspec) to download this zip file.

    ```rb
    {...}

    m.source = {
        :http => "http://{...}/mapbox-ios-sdk.zip",
        :flatten => true
    }

    {...}
    ```

1. Update your app's `Podfile` to point to the `Mapbox-iOS-SDK.podspec`.

    ```rb
    pod 'Mapbox-iOS-SDK', :podspec => 'http://{...}/Mapbox-iOS-SDK.podspec'
    ```

1. Run `pod update` to grab the newly-built library.



##### Testing Pre-Releases with CocoaPods

To test pre-releases and/or betas, you can reference the pre-release like so in your Podfile:

    pod 'Mapbox-iOS-SDK', podspec: 'https://raw.githubusercontent.com/mapbox/mapbox-gl-native/<insert branch or tag>/ios/Mapbox-iOS-SDK.podspec'


#### Dynamic framework

This is the recommended workflow for manually integrating the SDK into an application targeting iOS 8 and above:

1. Build from source manually per above.

1. Open the project editor and select your application target. Drag `build/ios/pkg/dynamic/Mapbox.framework` into the “Embedded Binaries” section of the General tab. (Don’t drag it into the “Linked Frameworks and Libraries” section; Xcode will add it there automatically.) In the sheet that appears, make sure “Copy items if needed” is checked, then click Finish.

1. In the Build Phases tab, click the + button at the top and select “New Run Script Phase”. Enter the following code into the script text field:

```bash
bash "${BUILT_PRODUCTS_DIR}/${FRAMEWORKS_FOLDER_PATH}/Mapbox.framework/strip-frameworks.sh"
```

(The last step, courtesy of [Realm](https://github.com/realm/realm-cocoa/), is required for working around an [iOS App Store bug](http://www.openradar.me/radar?id=6409498411401216) when archiving universal binaries.)

#### Static framework

If your application targets iOS 7.x, you’ll need to install the static framework instead:

1. Build from source manually per above.

1. Drag the Mapbox.bundle and Mapbox.framework files in `build/ios/pkg/static` into the Project navigator, checking "Copy items if needed". It should happen automatically, but ensure that:
   
   - `Mapbox.framework` is listed in your `Link Binary With Libraries` build phase.
   - Your *Framework Search Paths* (`FRAMEWORK_SEARCH_PATHS`) build setting includes the directory that contains `Mapbox.framework`. For most projects, the default value of `$(inherited) $(PROJECT_DIR)` should be sufficient.  
   - `Mapbox.bundle` is in your target's *Copy Bundle Resources* build phase.

1. **Optional** As [noted in the documentation](https://www.mapbox.com/ios-sdk/#telemetry_opt_out) you may use the provided Settings.bundle to provide a Telemetry opt out for users. To use the provided file, drag the `Settings.bundle` file in `build/ios/pkg` into the Project navigator, checking "Copy items if needed". It should happen automatically, but ensure that:

   - `Settings.bundle` is in your target's *Copy Bundle Resources* build phase. 

1. Add the following Cocoa Touch frameworks and libraries to the “Linked Frameworks and Libraries” section:

   - `GLKit.framework`
   - `ImageIO.framework`
   - `MobileCoreServices.framework`
   - `QuartzCore.framework`
   - `SystemConfiguration.framework`
   - `libc++.tbd`
   - `libsqlite3.tbd`
   - `libz.tbd`

1. In the Build Settings tab, add `-ObjC` to the “Other Linker Flags” (`OTHER_LDFLAGS`) build setting.

### Use

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

1. In a XIB or storyboard, add a View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the `Mapbox` module (Swift) or `Mapbox.h` umbrella header (Objective-C), then connect the map view to a new outlet in your view controller class. The resulting outlet declaration should look something like:

```objc
// ViewController.m
#import <Mapbox/Mapbox.h>

@interface ViewController : UIViewController

@property (strong) IBOutlet MGLMapView *mapView;

@end
```

```swift
// ViewController.swift
import Mapbox

class ViewController: UIViewController {
    @IBOutlet var mapView: MGLMapView!
}
```

### Troubleshooting

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.