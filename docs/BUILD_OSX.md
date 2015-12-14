# Building Mapbox GL Native for OS X

This project provides an OS X SDK analogous to the Mapbox iOS SDK. Mapbox does not officially support it to the same extent as the iOS SDK; however, bug reports and pull requests are certainly welcome. This document explains how to build the OS X SDK and integrate it into your own Cocoa application.

### Build

1. Run `make xpackage`, which produces a `Mapbox.framework` in the `gyp/build/Release/` folder.

### Install

1. Copy `gyp/build/Release/Mapbox.framework` into your project.

1. In the project editor, select your application target, go to the General tab, and add `Mapbox.framework` to the *Embedded Binaries* section.

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/).

1. In a XIB or storyboard, add a Custom View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the `Mapbox` module (Swift) or `Mapbox.h` umbrella header (Objective-C).

## Troubleshooting

You can also try clearing the Xcode cache with `make clear_xcode_cache`.
