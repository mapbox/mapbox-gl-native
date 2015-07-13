# Changelog

## 0.5.1

### iOS

- Added support for CocoaPods 0.38.0. ([#1876](https://github.com/mapbox/mapbox-gl-native/pull/1876))

## 0.5.0

### Core

- Support for runtime marker imagery. ([#941](https://github.com/mapbox/mapbox-gl-native/pull/941))
- Added `Map::fitBounds()` for region-based viewport setting. ([#1092](https://github.com/mapbox/mapbox-gl-native/issues/1092))
- Added a raster satellite bundled style and improved raster rendering. ([#963](https://github.com/mapbox/mapbox-gl-native/issues/963))
- Improved round line joins for semi-transparent lines. ([#1839](https://github.com/mapbox/mapbox-gl-native/pull/1839))
- Improved map render lifecycle notifications. ([#1026](https://github.com/mapbox/mapbox-gl-native/issues/1026))
- Fixed a bug that caused annotations not to show at zoom level zero. ([#1279](https://github.com/mapbox/mapbox-gl-native/issues/1279))
- Fixed a bug with the ordering of shape layers. ([#1866](https://github.com/mapbox/mapbox-gl-native/pull/1866))
- Other bug fixes and performance improvements. 

### iOS

- **Breaking:** Headers now make use of lightweight generics, eliminating many unnecessary casts when working with annotations in Swift 2.0 in Xcode 7. ([#1711](https://github.com/mapbox/mapbox-gl-native/pull/1711))
- **Breaking:** `-mapView:symbolNameForAnnotation:` has been removed from the `MGLMapViewDelegate` protocol. Implement `-mapView:imageForAnnotation:` instead, which accepts images at runtime. ([#941](https://github.com/mapbox/mapbox-gl-native/pull/941))
- **Breaking:** `MGLMapView.direction` is now expressed in terms of degrees clockwise from true north, as indicated in the documentation, rather than counterclockwise. ([#1789](https://github.com/mapbox/mapbox-gl-native/pull/1789))
- A Satellite style showing Mapbox Satellite imagery is now bundled with Mapbox GL. ([#1845](https://github.com/mapbox/mapbox-gl-native/pull/1845))
- Improved `UIView` tracking to the map. ([#1813](https://github.com/mapbox/mapbox-gl-native/pull/1813))
- Delegate method `-[MGLMapViewDelegate mapView:didFailToLocateUserWithError:]` now works. ([#1608](https://github.com/mapbox/mapbox-gl-native/pull/1608))
- It is now possible to fit the map’s viewport to a coordinate bounding box via `-[MGLMapView setVisibleCoordinateBounds:animated:]` or to a specific set of coordinates via `-[MGLMapView setVisibleCoordinates:count:edgePadding:animated:]`. ([#1783](https://github.com/mapbox/mapbox-gl-native/pull/1783), [#1795](https://github.com/mapbox/mapbox-gl-native/pull/1795))
- The logo and ℹ️ no longer disappear or get distorted after embedding MGLMapView in a different view, and you can now access these subviews directly via properties on MGLMapView. ([#1779](https://github.com/mapbox/mapbox-gl-native/pull/1779), [#1815](https://github.com/mapbox/mapbox-gl-native/pull/1815))
- Raster tiles now look sharper midway between two zoom levels. ([#1843](https://github.com/mapbox/mapbox-gl-native/pull/1843))
- Resetting the map rotation to north no longer also resets the user location tracking mode. ([#1809](https://github.com/mapbox/mapbox-gl-native/pull/1809))
- `-[MGLMapView convertPoint:toCoordinateFromView:]` now returns accurate coordinates on iPhone 6. ([#1827](https://github.com/mapbox/mapbox-gl-native/pull/1827))
- Fixed an issue in which `-[MGLMapView direction]` would sometimes return 360 instead of 0. ([#1829](https://github.com/mapbox/mapbox-gl-native/pull/1829))
- Build against iOS 8.4. ([#1868](https://github.com/mapbox/mapbox-gl-native/pull/1868))

## 0.4.0

### Core

- Support for polyline and polygon shape annotations. ([#1655](https://github.com/mapbox/mapbox-gl-native/issues/1655))
- Improved placement and density of labels. ([#1666](https://github.com/mapbox/mapbox-gl-native/issues/1666), [blog](https://www.mapbox.com/blog/better-label-placement-mapbox-mobile/))
- Improved z-ordering appearance of point markers. ([#988](https://github.com/mapbox/mapbox-gl-native/issues/988))
- Fixed an issue in which certain features, such as roundabouts, were not rendered completely. ([#1725](https://github.com/mapbox/mapbox-gl-native/issues/1725))
- Many bug fixes and performance and stability improvements.
- Improved tests.

### iOS

- **Breaking:** `MGLMapView` no longer manages Mapbox access tokens directly; an access token cannot be passed in when initializing the map view. Instead, set `MGLMapboxAccessToken` to your access token in your app’s `Info.plist` file, or call `+[MGLAccountManager setAccessToken:]` before initializing the map view. If you were setting the access token inside an Interface Builder inspectable, also remove it from the User Defined Runtime Attributes section of the Identity inspector. ([#1553](https://github.com/mapbox/mapbox-gl-native/issues/1553))
- **Breaking:** `MGLAccountManager`'s `-setMapboxMetricsEnabledSettingShownInApp:` has been removed. If you implement a Mapbox Metrics switch inside your app, instead of inside a Settings bundle, set `MGLMapboxMetricsEnabledSettingShownInApp` to `YES` in the `Info.plist` file. ([#1553](https://github.com/mapbox/mapbox-gl-native/issues/1553))
- **Breaking:** `MGLMapView`'s `-mapID` has been renamed to `-styleID`. ([#1561](https://github.com/mapbox/mapbox-gl-native/issues/1561))
- Headers have been audited for nullability, improving type safety in both Objective-C and Swift 1.2 when compiling with Xcode 6.3 or above. ([#1578](https://github.com/mapbox/mapbox-gl-native/issues/1578))
- Fixed an issue in which the map would sometimes spin 180° while rotating the map with two fingers. ([#1453](https://github.com/mapbox/mapbox-gl-native/issues/1453))
- Added a shortcut to the Mapbox Metrics switch in `MGLMapView`'s action sheet that is attached to the ℹ️ button. ([#1611](https://github.com/mapbox/mapbox-gl-native/issues/1611))
- `MGLMapView` now supports Interface Builder designables. When you add an `MGLMapView` to a storyboard, it displays instructions for getting set up directly on the storyboard canvas. ([#1573](https://github.com/mapbox/mapbox-gl-native/issues/1573))
- The default title for the user location annotation is now “You Are Here”. You can customize the title by setting `mapView.userAnnotation.title`. ([#1559](https://github.com/mapbox/mapbox-gl-native/issues/1559))
- Internal use of the Reachability library has been cleaned up so that your app can include its own copy of Reachability. ([#1718](https://github.com/mapbox/mapbox-gl-native/issues/1718))
- Now distribute a binary stripped of debugging symbols by default with an optional, secondary symbols build. ([#1650](https://github.com/mapbox/mapbox-gl-native/issues/1650))

## 0.3.1

- Temporarily removed `IBDesignable` support on iOS. 

## 0.3.0

- Initial iOS beta release. 

Known issues: 

- None. 
