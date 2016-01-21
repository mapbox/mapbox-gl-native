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

1. Run `make ipackage`. The packaging script will produce the statically-linked `libMapbox.a`, `Mapbox.bundle` for resources, a `Headers` folder, and a `Docs` folder with HTML API documentation.

### Install

There are two ways to install the Mapbox iOS SDK:

#### CocoaPods

Currently, until [#1437](https://github.com/mapbox/mapbox-gl-native/issues/1437) is completed, to install a _development version_ of Mapbox GL using CocoaPods you will need to build it from source manually per above.

1. Zip up the build product.

    ```
    cd build/ios/pkg/static
    ZIP=mapbox-ios-sdk.zip
    rm -f ../${ZIP}
    zip -r ../${ZIP} *
    ```

1. Modify a custom `Mapbox-iOS-SDK.podspec` to download this zip file.

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

#### Binary

1. Built from source manually per above.

1. Copy the contents of `build/ios/pkg/static` into your project. It should happen automatically, but ensure that:

   - `Headers` is in your *Header Search Paths* (`HEADER_SEARCH_PATHS`) build setting.
   - `Mapbox.bundle` is in your target's *Copy Bundle Resources* build phase.
   - `libMapbox.a` is in your target's *Link Binary With Libraries* build phase.

1. Add the following Cocoa framework dependencies to your target's *Link Binary With Libraries* build phase:

   - `GLKit.framework`
   - `ImageIO.framework`
   - `MobileCoreServices.framework`
   - `QuartzCore.framework`
   - `SystemConfiguration.framework`
   - `libc++.dylib`
   - `libsqlite3.dylib`
   - `libz.dylib`

1. Add `-ObjC` to your target's "Other Linker Flags" build setting (`OTHER_LDFLAGS`).

## Use

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

1. In a XIB or storyboard, add a View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the `Mapbox` module (Swift) or `Mapbox.h` umbrella header (Objective-C).

## Troubleshooting

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.
