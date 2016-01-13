# Integrating the Mapbox iOS SDK into your application

This document explains how to build a development version of Mapbox iOS SDK for the purpose of incorporating it into your own Cocoa Touch application.

### Requirements

The Mapbox iOS SDK and iosapp demo application build against the iOS 7.0 SDK. They are intended to run on iOS 7.0 and above on the following devices and their simulators:

* iPhone 4S and above (5, 5c, 5s, 6, 6 Plus)
* iPad 2 and above (3, 4, Mini, Air, Mini 2, Air 2)
* iPod touch 5th generation and above

### Build

1. [Install core dependencies](../../INSTALL.md).
  * For development on OS X, you can install pkg-config using `brew install pkg-config`

1. Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

   ```
   [sudo] gem install jazzy
   ```

1. From the path at the root of the project, e.g. `/path/to/mapbox-gl-native $ `, run
```
  make ipackage  #  makes ./build/ios/pkg/static/
```
 The packaging script will produce the statically-linked `libMapbox.a`, `Mapbox.bundle` for resources, a `Headers` folder, and a `Docs` folder with HTML API documentation.

 ![makes ./build/ios/pkg/static/](libMapbox.a.png)

1.  In addition to `make ipackage`, other make commands include
```
make iproj    # makes ./build/ios-all/gyp/ios.xcodeproj/
```
After opening `ios.xcodeproj`, you will see a couple of targets

  ![Open the project ios.xcodeproj](ios.xcodeproj.png)  

  ![Targets within ios.xcodeproj](ios.xcodeproj.targets.png)

### Access Tokens

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

Set up the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)

### Test

In the context of your own app, you can now either:

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

## Troubleshooting

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.
