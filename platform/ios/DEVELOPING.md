# Contributing to the Mapbox iOS SDK

This document explains how to build the Mapbox iOS SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

### Requirements

The Mapbox iOS SDK and iosapp demo application build against the iOS 7.0 SDK and require Xcode on a computer running OS X.

### Build

[Install core dependencies](../../INSTALL.md), then run

    make iproj

Which will create and open an Xcode workspace that includes both the SDK source and some Objective-C test applications. Before building, use the scheme picker button in the toolbar to create a `New Scheme` and then select `iosapp` as the target and use the default "iosapp" as the name and click `Ok`. Finally, change the scheme to the new “iosapp” and the destination to one of the simulators or connected devices listed in the menu.

If you don't have an Apple Developer account, change the destination to a simulator such as "iPhone 6" before you run and build the app.

`ipackage` is only one of several available `make` build targets. Others include:

* `make ipackage` builds both dynamic and static frameworks in the Release configuration for devices and the iOS Simulator.
* `make ipackage-sim` builds a dynamic framework in the Debug configuration for the iOS simulator. This is the fastest target.
* `make ipackage-strip` builds both dynamic and static frameworks in the Release configuration, stripped of debug symbols, for devices and the iOS Simulator.
* `make iframework` builds a dynamic framework in the Release configuration for devices and the iOS Simulator. The CocoaPods pod downloads the output of this target.

You can customize the build output by passing the following arguments into the `make` invocation:

* `BUILD_DEVICE=false` builds only for the iOS Simulator.
* `FORMAT=dynamic` builds only a dynamic framework. `FORMAT=static` builds only a static framework, for compatibility with iOS 7.x.
* `SYMBOLS=NO` strips the build output of any debug symbols, yielding much smaller binaries.

### Access tokens

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

Set up the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)

### Test

Run

    make test-ios

To run the included unit tests and integration tests on the command line.

To instead run the tests in Xcode:

1. Run `make iproj` to set up the workspace.
1. Change the scheme to “test (platform project)” and press Command-R to run core unit tests.
1. Change the scheme to “CI” and press Command-U to run SDK integration tests.

### iosapp usage

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)
- Use the debug menu to add test annotations, reset position, and cycle through the debug options.
