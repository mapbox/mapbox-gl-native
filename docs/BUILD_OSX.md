# Building Mapbox GL Native for OS X

This project provides an OS X SDK analogous to the Mapbox iOS SDK. Mapbox does not officially support it to the same extent as the iOS SDK; however, bug reports and pull requests are certainly welcome. This document explains how to build the OS X SDK and integrate it into your own Cocoa application.

### Build

1. Run `make xpackage`. The packaging script will produce the statically-linked `libMapbox.a`, `Mapbox.bundle` for resources, and a `Headers` folder.

### Install

1. Copy the contents of `build/osx/pkg/static` into your project. It should happen automatically, but ensure that:

   - `Headers` is in your *Header Search Paths* (`HEADER_SEARCH_PATHS`) build setting.
   - `Mapbox.bundle` is in your target's *Copy Bundle Resources* build phase.
   - `libMapbox.a` is in your target's *Link Binary With Libraries* build phase.

1. Add the following Cocoa framework dependencies to your target's *Link Binary With Libraries* build phase:

   - `SystemConfiguration.framework`
   - `libc++.tbd`
   - `libsqlite3.tbd`
   - `libz.tbd`

1. Add `-ObjC` to your target's "Other Linker Flags" build setting (`OTHER_LDFLAGS`).

1. Mapbox vector tiles require a Mapbox account and API access token. In the project editor, select the application target. In the Info tab, set `MGLMapboxAccessToken` to your access token. You can obtain one from the [Mapbox account page](https://www.mapbox.com/account/apps/).

1. In a XIB or storyboard, add a Custom View and set its custom class to `MGLMapView`. If you need to manipulate the map view programmatically, import the `Mapbox` module (Swift) or `Mapbox.h` umbrella header (Objective-C).

## Troubleshooting

You can also try clearing the Xcode cache with `make clear_xcode_cache`.
