# Contributing to the Mapbox iOS SDK

This document explains how to build the Mapbox iOS SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

### Requirements

The Mapbox iOS SDK and iosapp demo application build against the iOS 7.0 SDK. The SDK is intended to run on iOS 7.0 and above, while iosapp is intended to run on iOS 8.0 and above due to the use of a dynamic framework. Both require Xcode on a computer running OS X.

### Build

[Install core dependencies](../../INSTALL.md), then run

```bash
make iproj
```

which will create and open an Xcode workspace that includes both the SDK source and some Objective-C test applications. Before building, use the scheme picker button in the toolbar to change the scheme to “iosapp” and the destination to one of the simulators or connected devices listed in the menu.

If you don’t have an Apple Developer account, change the destination to a simulator such as “iPhone 6s” before you run and build the app.

`iproj` is only one of several available `make` rules. Others include:

* `make ipackage` builds both dynamic and static frameworks in the Release configuration for devices and the iOS Simulator.
* `make ipackage-sim` builds a dynamic framework in the Debug configuration for the iOS simulator. This is the fastest target.
* `make ipackage-strip` builds both dynamic and static frameworks in the Release configuration, stripped of debug symbols, for devices and the iOS Simulator.
* `make iframework` builds a dynamic framework in the Release configuration for devices and the iOS Simulator. The CocoaPods pod downloads the output of this target.

You can customize the build output by passing the following arguments into the `make` invocation:

* `BUILD_DEVICE=false` builds only for the iOS Simulator.
* `FORMAT=dynamic` builds only a dynamic framework. `FORMAT=static` builds only a static framework, for compatibility with iOS 7.x.
* `SYMBOLS=NO` strips the build output of any debug symbols, yielding much smaller binaries.

These targets require that you install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

```bash
[sudo] gem install jazzy
```

### Access tokens

The included applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo or benchmarking application. In the iOS Simulator, you can paste your access token by pressing Command-V, holding down the left mouse button in the text field, and pressing the Paste button that appears.

### Test

`make test-ios` builds and runs unit tests of cross-platform code as well as the SDK.

To run these unit tests on the command line, install ios-sim version 3.2.0 (not any other version):

```bash
brew tap mapbox/homebrew-ios-sim-3
brew install mapbox/homebrew-ios-sim-3/ios-sim
```

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
