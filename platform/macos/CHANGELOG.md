# Changelog for Mapbox macOS SDK

## master

* Right-clicking to open MGLMapView’s context menu no longer prevents the user from subsequently panning the map by clicking and dragging. ([#5593](https://github.com/mapbox/mapbox-gl-native/pull/5593))
* Replaced the wireframe debug mask with an overdraw visualization debug mask to match Mapbox GL JS’s overdraw inspector. ([#5403](https://github.com/mapbox/mapbox-gl-native/pull/5403))
* Improved the design of the generated API documentation. ([#5306](https://github.com/mapbox/mapbox-gl-native/pull/5306))

## 0.2.0

* This version of the Mapbox macOS SDK roughly corresponds to version 3.3.0-beta.1 of the Mapbox iOS SDK. The two SDKs have very similar feature sets. The main differences are the lack of user location tracking and annotation views. Some APIs have been adapted to macOS conventions, particularly the use of NSPopover for callout views.
* Renamed the SDK to the Mapbox macOS SDK.
* Fixed an issue in which Mapbox.framework was nested inside another folder named Mapbox.framework. ([#4998](https://github.com/mapbox/mapbox-gl-native/pull/4998))
* Added methods to MGLMapView for obtaining the underlying map data rendered by the current style, along with additional classes to represent complex geometry in that data. ([#5110](https://github.com/mapbox/mapbox-gl-native/pull/5110))
* An MGLPolygon can now have interior polygons, representing holes knocked out of the overall shape. ([#5110](https://github.com/mapbox/mapbox-gl-native/pull/5110))
* Fixed a vector tile parsing bug that sometimes caused properties in the vector tile source to be mismatched. ([#5183](https://github.com/mapbox/mapbox-gl-native/pull/5183))
* Fixed a crash passing a mixture of point and shape annotations into `-[MGLMapView addAnnotations:]`. ([#5097](https://github.com/mapbox/mapbox-gl-native/pull/5097))
* Fixed an issue (speculatively) where the tile cache could be included in iCloud backups. ([#5124](https://github.com/mapbox/mapbox-gl-native/pull/5124))
* Improved performance viewing regions with large landcover polygons when viewing a style that uses the Mapbox Streets source. ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))
* Fixed a memory leak when using raster resources. ([#5141](https://github.com/mapbox/mapbox-gl-native/pull/5141))
* Added `MGLCoordinateInCoordinateBounds()`, a function that tests whether or not a coordinate is in a given bounds. ([#5053](https://github.com/mapbox/mapbox-gl-native/pull/5053))
* Fixed an issue in which fade transitions (such as on street labels in some styles) lagged behind the map when quickly zooming in and out. ([#4579](https://github.com/mapbox/mapbox-gl-native/pull/4579))
* Added new options to `MGLMapDebugMaskOptions` that show wireframes and the stencil buffer instead of the color buffer. ([#4359](https://github.com/mapbox/mapbox-gl-native/pull/4359))
* Declarations in the API documentation are shown in both Objective-C and Swift. ([realm/jazzy#530](https://github.com/realm/jazzy/pull/530))

## 0.1.0

* This version of the Mapbox OS X SDK roughly corresponds to version 3.3.0-alpha.2 of the Mapbox iOS SDK. The two SDKs have very similar feature sets. The main difference is the lack of user location tracking. Some APIs have been adapted to OS X conventions, particularly the use of NSPopover for callout views.
