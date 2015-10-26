# Building Mapbox GL Native for iOS

This section is for people contributing to Mapbox GL directly in the context of their own app.

### Build

1. Install [appledoc](http://appledoc.gentlebytes.com/appledoc/) for API docs generation.

   ```
   curl -L -o appledoc.zip https://github.com/tomaz/appledoc/releases/download/v2.2-963/appledoc.zip
   unzip appledoc.zip
   cp appledoc /usr/local/bin
   cp -Rf Templates/ ~/.appledoc
   ```

1. Run `make ipackage`. The packaging script will produce the statically-linked `libMapbox.a`, `Mapbox.bundle` for resources, a `Headers` folder, and a `Docs` folder with HTML API documentation.

### Access Tokens

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

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
   - `CoreTelephony.framework` (optional, telemetry-only)

1. Add `-ObjC` to your target's "Other Linker Flags" build setting (`OTHER_LDFLAGS`).

## Troubleshooting

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.
