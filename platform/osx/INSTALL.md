# Integrating the Mapbox OS X SDK into your application

This document explains how to build the Mapbox OS X SDK and integrate it into your own Cocoa application.

### Requirements

The Mapbox OS X SDK requires the OS X 10.10.0 SDK or above.

### Build

1. [Install core dependencies](../../INSTALL.md).
1. Run `make xpackage`, which produces a `Mapbox.framework` in the `gyp/build/Release/` folder.

### Install

1. Copy `gyp/build/Release/Mapbox.framework` into your project.

1. In the project editor, select your application target, go to the General tab, and add `Mapbox.framework` to the *Embedded Binaries* section.

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

1. In a XIB or storyboard, add a Custom View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the `Mapbox` module (Swift) or `Mapbox.h` umbrella header (Objective-C).

## Use

The [Mapbox iOS SDK’s API documentation](https://www.mapbox.com/ios-sdk/api/) applies to the Mapbox OS X SDK with few differences, mostly around unimplemented features like user location tracking.

## Troubleshooting

You can also try clearing the Xcode cache with `make clear_xcode_cache`.
