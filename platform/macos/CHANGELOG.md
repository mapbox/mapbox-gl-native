# Changelog for Mapbox macOS SDK

## master

* Fixed an issue causing code signing failures and bloating the framework. ([#5850](https://github.com/mapbox/mapbox-gl-native/pull/5850))
* Xcode 7.3 or higher is now required for using this SDK. ([#6059](https://github.com/mapbox/mapbox-gl-native/issues/6059))
* A new runtime styling API allows you to adjust the style and content of the base map dynamically. All the options available in [Mapbox Studio](https://www.mapbox.com/studio/) are now exposed via MGLStyle and subclasses of MGLStyleLayer and MGLSource. ([#5727](https://github.com/mapbox/mapbox-gl-native/pull/5727))
* Improved offline and ambient cache database performance. ([#5796](https://github.com/mapbox/mapbox-gl-native/pull/5796))
* Added `showAnnotations:animated:` and `showAnnotations:edgePadding:animated:`, which moves the map viewport to show the specified annotations. ([#5749](https://github.com/mapbox/mapbox-gl-native/pull/5749))
* Fixed an issue where the map view’s center would always be calculated as if the view occupied the entire window. ([#6102](https://github.com/mapbox/mapbox-gl-native/pull/6102))
* To make an MGLPolyline or MGLPolygon span the antimeridian, specify coordinates with longitudes greater than 180° or less than −180°. ([#6088](https://github.com/mapbox/mapbox-gl-native/pull/6088))
* MGLMapView’s `styleURL` property can now be set to an absolute file URL. ([#6026](https://github.com/mapbox/mapbox-gl-native/pull/6026))
* GeoJSON sources specified by the stylesheet at design time now support `cluster`, `clusterMaxZoom`, and `clusterRadius` attributes for clustering point features on the base map. ([#5724](https://github.com/mapbox/mapbox-gl-native/pull/5724))
* TileJSON manifests can now specify `"scheme": "tms"` to indicate the use of [TMS](https://en.wikipedia.org/wiki/Tile_Map_Service) coordinates. ([#2270](https://github.com/mapbox/mapbox-gl-native/pull/2270))
* Fixed rendering artifacts and missing glyphs that occurred after viewing a large number of CJK characters on the map. ([#5908](https://github.com/mapbox/mapbox-gl-native/pull/5908))
* Improved the precision of annotations at zoom levels greater than 18. ([#5517](https://github.com/mapbox/mapbox-gl-native/pull/5517))
* Fixed an issue where the style zoom levels were not respected when deciding when to render a layer. ([#5811](https://github.com/mapbox/mapbox-gl-native/issues/5811))
* If MGLMapView is unable to obtain or parse a style, it now calls its delegate’s `-mapViewDidFailLoadingMap:withError:` method. ([#6145](https://github.com/mapbox/mapbox-gl-native/pull/6145))
* Fixed crashes that could occur when loading a malformed stylesheet. ([#5736](https://github.com/mapbox/mapbox-gl-native/pull/5736))
* Fixed a typo in the documentation for the MGLCompassDirectionFormatter class. ([#5879](https://github.com/mapbox/mapbox-gl-native/pull/5879))

## 0.2.1 - July 19, 2016

* Fixed a crash that occurred when a sprite URL lacks a file extension. See [this comment](https://github.com/mapbox/mapbox-gl-native/issues/5722#issuecomment-233701251) to determine who may be affected by this bug. ([#5723](https://github.com/mapbox/mapbox-gl-native/pull/5723))
* Right-clicking to open MGLMapView’s context menu no longer prevents the user from subsequently panning the map by clicking and dragging. ([#5593](https://github.com/mapbox/mapbox-gl-native/pull/5593))
* Fixed an issue causing overlapping polylines and polygons to be drawn in undefined z-order. Shapes are always drawn in the order they are added to the map, from the oldest on the bottom to the newest on the top. ([#5710](https://github.com/mapbox/mapbox-gl-native/pull/5710))
* Improved the design of the generated API documentation. ([#5306](https://github.com/mapbox/mapbox-gl-native/pull/5306))
* As the user zooms in, tiles from lower zoom levels are scaled up until tiles for higher zoom levels are loaded. ([#5143](https://github.com/mapbox/mapbox-gl-native/pull/5143))
* Per documentation, the first and last coordinates in an MGLPolygon must be identical in order for the polygon to draw correctly. The same is true for an MGLPolygon’s interior polygon. ([#5514](https://github.com/mapbox/mapbox-gl-native/pull/5514))
* Added [quadkey](https://msdn.microsoft.com/en-us/library/bb259689.aspx) support and limited WMS support in raster tile URL templates. ([#5628](https://github.com/mapbox/mapbox-gl-native/pull/5628))
* Fixed a crash that occurred when a style or other resource URL has a query string. ([#5554](https://github.com/mapbox/mapbox-gl-native/pull/5554))
* Fixed an issue causing polyline and polygon annotations to disappear when the zoom level is one less than the maximum zoom level. ([#5418](https://github.com/mapbox/mapbox-gl-native/pull/5418))
* Added a property to MGLOfflineStorage, `countOfBytesCompleted`, that indicates the disk space occupied by all cached and offline resources. ([#5585](https://github.com/mapbox/mapbox-gl-native/pull/5585))
* The `text-pitch-alignment` property is now supported in stylesheets for improved street label legibility on a tilted map. ([#5288](https://github.com/mapbox/mapbox-gl-native/pull/5288))
* The `icon-text-fit` and `icon-text-fit-padding` properties are now supported in stylesheets, allowing the background of a shield to automatically resize to fit the shield’s text. ([#5334](https://github.com/mapbox/mapbox-gl-native/pull/5334))
* The `circle-pitch-scale` property is now supported in stylesheets, allowing circle features in a tilted base map to scale or remain the same size as the viewing distance changes. ([#5576](https://github.com/mapbox/mapbox-gl-native/pull/5576))
* The `identifier` property of an MGLFeature may now be either a number or string. ([#5514](https://github.com/mapbox/mapbox-gl-native/pull/5514))
* Improved the performance of relocating a point annotation by changing its `coordinate` property. ([#5385](https://github.com/mapbox/mapbox-gl-native/pull/5385))
* Replaced the wireframe debug mask with an overdraw visualization debug mask to match Mapbox GL JS’s overdraw inspector. ([#5403](https://github.com/mapbox/mapbox-gl-native/pull/5403))
* MGLMapDebugOverdrawVisualizationMask and MGLMapDebugStencilBufferMask no longer have any effect in Release builds of the SDK. These debug masks have been disabled for performance reasons. ([#5555](https://github.com/mapbox/mapbox-gl-native/pull/5555))

## 0.2.0 - June 14, 2016

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

## 0.1.0 - May 10, 2016

* This version of the Mapbox OS X SDK roughly corresponds to version 3.3.0-alpha.2 of the Mapbox iOS SDK. The two SDKs have very similar feature sets. The main difference is the lack of user location tracking. Some APIs have been adapted to OS X conventions, particularly the use of NSPopover for callout views.
