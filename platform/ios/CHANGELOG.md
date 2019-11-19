# Changelog for Mapbox Maps SDK for iOS

Mapbox welcomes participation and contributions from everyone. Please read [CONTRIBUTING.md](../../CONTRIBUTING.md) to get started.

## master

### Other changes

### Bug fixes
* Fixed the rendering bug caused by redundant pending requests for already requested images [#15864](https://github.com/mapbox/mapbox-gl-native/pull/15864)
* Enable incremental vacuum for the offline database in order to make data removal requests faster and to avoid the excessive disk space usage (creating a backup file on VACUUM call). ([#15837](https://github.com/mapbox/mapbox-gl-native/pull/15837))
* Fixed Geo JSON source flickering on style transition. ([#15907](https://github.com/mapbox/mapbox-gl-native/pull/15907))
* Fixed flickering caused by unnecessary removing and re-adding of the render sources when the order of their corresponding style objects was changed in the updated style ([#15941](https://github.com/mapbox/mapbox-gl-native/pull/15941))

## 5.5.0

### Performance improvements

* Improved rendering performance for the styles with multiple sources ([#15756](https://github.com/mapbox/mapbox-gl-native/pull/15756))

### Styles and rendering

* Added an `MGLShapeSourceOptionClusterProperties` option that allows styling individual clusters based on aggregated feature data. ([#15515](https://github.com/mapbox/mapbox-gl-native/pull/15515))

### Other changes

* Added `-[MGLMapSnapshotOverlay coordinateForPoint:]` and `-[MGLMapSnapshotOverlay pointForCoordinate:]` to convert between context and map coordinates, mirroring those of `MGLMapSnapshot`. ([#15746](https://github.com/mapbox/mapbox-gl-native/pull/15746))
* Suppress network requests for expired tiles update, if these tiles are invisible. ([#15741](https://github.com/mapbox/mapbox-gl-native/pull/15741))
* Fixed an issue that cause the ornaments to ignore `MGLMapView.contentInset` property. ([#15584](https://github.com/mapbox/mapbox-gl-native/pull/15584))
* Fixed an issue that cause `-[MGLMapView setCamere:withDuration:animationTimingFunction:edgePadding:completionHandler:]` persist the value of `edgePadding`. ([#15584](https://github.com/mapbox/mapbox-gl-native/pull/15584))
* Added `MGLMapView.automaticallyAdjustsContentInset` property that indicates if wether the map view should automatically adjust its content insets. ([#15584](https://github.com/mapbox/mapbox-gl-native/pull/15584))
* Fixed an issue that caused `MGLScaleBar` to have an incorrect size when resizing or rotating. ([#15703](https://github.com/mapbox/mapbox-gl-native/pull/15703))
* Fixed crash at launch seen on iOS 9 simulator. ([#15771](https://github.com/mapbox/mapbox-gl-native/pull/15771))

## 5.4.0 - September 25, 2019

### Styles and rendering

* Added an `-[MGLMapSnapshotter startWithOverlayHandler:completionHandler:]` method to provide the snapshot's current `CGContext` in order to perform custom drawing on `MGLMapSnapshot` objects. ([#15530](https://github.com/mapbox/mapbox-gl-native/pull/15530))
* Fixed an issue that caused `MGLTileSourceOptionMaximumZoomLevel` to be ignored when setting `MGLTileSource.configurationURL`. ([#15581](https://github.com/mapbox/mapbox-gl-native/pull/15581))
* Fixed an assertion hit caused by possibility of adding a layer to an incompatible source. ([#15644](https://github.com/mapbox/mapbox-gl-native/pull/15644))
* Fixed crashes triggered when `MGLSource` and `MGLStyleLayer` objects are accessed after having been invalidated after a style change. ([#15539](https://github.com/mapbox/mapbox-gl-native/pull/15539))
* Fixed an issue where the collision boxes for symbols would not be updated when `MGLSymbolStyleLayer.textTranslation` or `MGLSymbolStyleLayer.iconTranslation` were used. ([#15467](https://github.com/mapbox/mapbox-gl-native/pull/15467))
* Enabled use of `MGLSymbolStyleLayer.textOffset` option together with `MGLSymbolStyleLayer.textVariableAnchor` (if `MGLSymbolStyleLayer.textRadialOffset` option is not provided). ([#15542](https://github.com/mapbox/mapbox-gl-native/pull/15542))
* Fixed an issue that caused constant repainting for sources with invisible layers. ([#15600](https://github.com/mapbox/mapbox-gl-native/pull/15600))

### User interaction

* Fixed an issue that caused the tilt gesture to trigger too easily and conflict with pinch or pan gestures. ([#15349](https://github.com/mapbox/mapbox-gl-native/pull/15349))
* Fixed an issue that caused the map to rotate too easily during a pinch gesture. [(#15562)](https://github.com/mapbox/mapbox-gl-native/pull/15562)

### Performance improvements
 
* Improved offline region download performance by batching certain database activities. ([#15521](https://github.com/mapbox/mapbox-gl-native/pull/15521))
* Newly loaded labels appear faster on the screen. ([#15308](https://github.com/mapbox/mapbox-gl-native/pull/15308))

### Other changes

* Fixed a bug where the completion block passed to `-[MGLMapView flyToCamera:completionHandler:]` (and related methods) wouldn't be called. ([#15473](https://github.com/mapbox/mapbox-gl-native/pull/15473))
* Fixed a crash when offline pack invalidation happened on different threads. ([#15582](https://github.com/mapbox/mapbox-gl-native/pull/15582))
* Fixed a potential integer overflow at high zoom levels. ([#15560](https://github.com/mapbox/mapbox-gl-native/pull/15560))
* Fixed a bug with annotation view positions after camera transitions. ([#15122](https://github.com/mapbox/mapbox-gl-native/pull/15122/))

## 5.3.2 - September 18, 2019

* Fixed an issue where `-[MGLMapView visibleFeaturesInRect:]` and `-[MGLShapeSource featuresMatchingPredicate:]` could return incorrect coordinates at zoom levels 20 and higher. ([#15560](https://github.com/mapbox/mapbox-gl-native/pull/15560))

## 5.3.1 - September 18, 2019

### Styles and rendering

* Fixed an issue where connecting coincident holes in a polygon could lead to a race condition. ([#15660](https://github.com/mapbox/mapbox-gl-native/pull/15660))

### Other changes

* Fixed an issue where the scale bar text would become illegible if iOS 13 dark mode was enabled. ([#15524](https://github.com/mapbox/mapbox-gl-native/pull/15524))
* Fixed an issue with the appearance of the compass text in iOS 13. ([#15547](https://github.com/mapbox/mapbox-gl-native/pull/15547))

## 5.3.0 - August 28, 2019

This release changes how offline tile requests are billed — they are now billed on a pay-as-you-go basis and all developers are able raise the offline tile limit for their users. Offline requests were previously exempt from monthly active user (MAU) billing and increasing the offline per-user tile limit to more than 6,000 tiles required the purchase of an enterprise license. By upgrading to this release, you are opting into the changes outlined in [this blog post](https://blog.mapbox.com/offline-maps-for-all-bb0fc51827be) and [#15380](https://github.com/mapbox/mapbox-gl-native/pull/15380).

### Styles and rendering

* Fixed flickering on style change for the same tile set. ([#15127](https://github.com/mapbox/mapbox-gl-native/pull/15127))
* Fixed performance issues when tilting a map with custom content insets. ([#15195](https://github.com/mapbox/mapbox-gl-native/pull/15195))
* Fixed an issue where animated camera transitions zoomed in or out too dramatically. ([#15281](https://github.com/mapbox/mapbox-gl-native/pull/15281))
* Enabled variable label placement when `MGLSymbolStyleLayer.textAllowsOverlap` is set to true. ([#15354](https://github.com/mapbox/mapbox-gl-native/pull/15354))
* Added the `MGLSymbolStyleLayer.textWritingModes` layout property. This property can be set to `MGLTextWritingModeHorizontal` or `MGLTextWritingModeVertical`. ([#14932](https://github.com/mapbox/mapbox-gl-native/pull/14932))
* Fixed rendering and collision detection issues with using `MGLSymbolStyleLayer.textVariableAnchor` and `MGLSymbolStyleLayer.iconTextFit` properties on the same layer. ([#15367](https://github.com/mapbox/mapbox-gl-native/pull/15367))
* Fixed symbol overlap when zooming out quickly. ([#15416](https://github.com/mapbox/mapbox-gl-native/pull/15416))
* Fixed a rendering issue where non-SDF icons would be treated as SDF icons if they are in the same layer. ([#15456](https://github.com/mapbox/mapbox-gl-native/pull/15456))

### Other changes

* Fixed a bug where glyphs generated through the LocalGlyphRasterizer interface were changing fonts during some zoom changes. ([#15407](https://github.com/mapbox/mapbox-gl-native/pull/15407))
* Fixed use of objects after moving, potentially causing crashes. ([#15408](https://github.com/mapbox/mapbox-gl-native/pull/15408))
* Fixed a possible crash that could be caused by invoking the wrong layer implementation casting function. ([#15398](https://github.com/mapbox/mapbox-gl-native/pull/15398))
* Fixed a rare crash when tile download requests returned “404 Not Found” errors. ([#15313](https://github.com/mapbox/mapbox-gl-native/pull/15313))
* `MGLLoggingLevel` has been updated to better match core log levels. You can now use `MGLLoggingConfiguration.loggingLevel` to filter logs from core. ([#15120](https://github.com/mapbox/mapbox-gl-native/pull/15120))
* Fixed an issue where the scale bar could show `0 mm` instead of `0`. ([#15381](https://github.com/mapbox/mapbox-gl-native/pull/15381))

## 4.11.2 - July 30, 2019

* Fixed a map update bug caused by the render tiles and the render passes becoming unsynchronized. ([#15092](https://github.com/mapbox/mapbox-gl-native/pull/15092))
* Fixed a custom geometry source bug caused by using the outdated tiles after style update. ([#15112](https://github.com/mapbox/mapbox-gl-native/pull/15112))

## 5.0.2 - July 29, 2019

* Fixed an issue where fill extrusion layers would be incorrectly rendered above other layers. ([#15065](https://github.com/mapbox/mapbox-gl-native/pull/15065))
* Fixed a map update bug caused by the render tiles and the render passes becoming unsynchronized. ([#15092](https://github.com/mapbox/mapbox-gl-native/pull/15092))
* Fixed a custom geometry source bug caused by using the outdated tiles after style update. ([#15112](https://github.com/mapbox/mapbox-gl-native/pull/15112))

## 5.1.2 - July 25, 2019

* Fixed a crash during network access. ([#15113](https://github.com/mapbox/mapbox-gl-native/pull/15113))		

## 5.2.0 - July 24, 2019

### Networking

* Added methods to clear the ambient cache, change the size of the ambient cache, and to delete and revalidate the database that contains the ambient cache and offline packs ([#14978](https://github.com/mapbox/mapbox-gl-native/pull/14978)):
  * `-[MGLOfflineStorage clearAmbientCacheWithCompletionHandler:]`
  * `-[MGLOfflineStorage invalidateAmbientCacheWithCompletionHandler:]`
  * `-[MGLOfflineStorage setMaximumAmbientCacheSize:withCompletionHandler:]`
  * `-[MGLOfflineStorage resetDatabaseWithCompletionHandler:]`
  * `-[MGLOfflineStorage invalidatePack:withCompletionHandler:]`

### Styles and rendering

* The `MGLIdeographicFontFamilyName` Info.plist key now also accepts an array of font family names, to customize font fallback behavior. It can also be set to a Boolean value of `NO` to force the SDK to typeset CJK characters in a remote font specified by `MGLSymbolStyleLayer.textFontNames`. ([#14862](https://github.com/mapbox/mapbox-gl-native/pull/14862))
* Hiragana and katakana are now included in the range of CJK glyphs that are rendered locally by default. ([#15009](https://github.com/mapbox/mapbox-gl-native/pull/15009))
* Fixed a map update bug caused by the render tiles and the render passes becoming unsynchronized. ([#15092](https://github.com/mapbox/mapbox-gl-native/pull/15092))
* Added the `-[MGLMapViewDelegate mapView:shouldRemoveStyleImage:]` method for optimizing style image caching. ([#14769](https://github.com/mapbox/mapbox-gl-native/pull/14769))
* Fixed style change transition regression caused by delayed setting of the updated layer properties. ([#15016](https://github.com/mapbox/mapbox-gl-native/pull/15016))
* Fixed an issue where layers with fill extrusions would be incorrectly rendered above other layers. ([#15065](https://github.com/mapbox/mapbox-gl-native/pull/15065))
* Improved feature querying performance. ([#14930](https://github.com/mapbox/mapbox-gl-native/pull/14930))
* Fixed a custom geometry source bug caused by using the outdated tiles after style update ([#15112](https://github.com/mapbox/mapbox-gl-native/pull/15112))

### User interaction

* Fixed an issue where annotations could not be selected. ([#15130](https://github.com/mapbox/mapbox-gl-native/pull/15130))
* Fixed a bug where using the pinch gesture could result in an incorrect map center coordinate. ([#15097](https://github.com/mapbox/mapbox-gl-native/pull/15097))
* Fixed an issue where the two-finger tilt gesture would continue after lifting one finger. ([#14969](https://github.com/mapbox/mapbox-gl-native/pull/14969))

### Offline maps

* Ideographic glyphs from Chinese, Japanese, and Korean are no longer downloaded by default as part of offline packs; they are instead rendered on-device, saving bandwidth and storage while improving performance. ([#14176](https://github.com/mapbox/mapbox-gl-native/pull/14176))
* Fixed an issue where offline regions could report the wrong number of tiles. ([#14958](https://github.com/mapbox/mapbox-gl-native/pull/14958))

### Packaging

* Removed previously deprecated methods and properties that had been marked `unavailable`. ([#15000](https://github.com/mapbox/mapbox-gl-native/pull/15000))
* The downloaded zip file of this framework no longer contains a local podspec. ([#15027](https://github.com/mapbox/mapbox-gl-native/pull/15027))

### Other changes

* Added variants of several animated `MGLMapView` methods that accept completion handlers ([#14381](https://github.com/mapbox/mapbox-gl-native/pull/14381)):
  * `-[MGLMapView setVisibleCoordinateBounds:edgePadding:animated:completionHandler:]`
  * `-[MGLMapView setContentInset:animated:completionHandler:]`
  * `-[MGLMapView setUserTrackingMode:animated:completionHandler:]`
  * `-[MGLMapView setTargetCoordinate:animated:completionHandler:]`
  * `-[MGLMapView showAnnotations:edgePadding:animated:completionHandler:]`
  * `-[MGLMapView selectAnnotation:animated:completionHandler:]`
* Deprecated variants of the above methods without completion handlers. ([#14959](https://github.com/mapbox/mapbox-gl-native/pull/14959))
* Added `MGLMapView.compassView.compassVisibility` and `MGLOrnamentVisibility` to allow configuration of compass visibility behavior. ([#15055](https://github.com/mapbox/mapbox-gl-native/pull/15055))
* Fixed a crash during network access. ([#15113](https://github.com/mapbox/mapbox-gl-native/pull/15113))
* Updated "map ID" to the more accurate term "tileset ID" in documentation; updated "style's Map ID" to the more accurate term "style URL". ([#15116](https://github.com/mapbox/mapbox-gl-native/pull/15116))

## 4.10.1 - July 19, 2019

* Fixed a bug in telemetry collection. ([#15077](https://github.com/mapbox/mapbox-gl-native/pull/15077))

## 4.11.1 - July 19, 2019

* Fixed a bug in telemetry collection. ([#15077](https://github.com/mapbox/mapbox-gl-native/pull/15077))

## 5.0.1 - July 18, 2019

* Fixed a bug in telemetry collection. ([#15077](https://github.com/mapbox/mapbox-gl-native/pull/15077))

## 5.1.1 - July 18, 2019

* Fixed a bug in telemetry collection. ([#15077](https://github.com/mapbox/mapbox-gl-native/pull/15077))

## 5.1.0 - June 19, 2019

### Styles and rendering

* Fixed a crash when a fill pattern in a style could not be found. ([#14696](https://github.com/mapbox/mapbox-gl-native/pull/14696))
* Fixed a rendering performance regression when rendering polylines. ([#14851](https://github.com/mapbox/mapbox-gl-native/pull/14851))
* Fixed a rendering performance regression introduced in 4.11.0. ([#14907](https://github.com/mapbox/mapbox-gl-native/pull/14907))
* Fixed an issue where symbols underneath opaque fill layers could be incorrectly drawn above such layers. ([#14839](https://github.com/mapbox/mapbox-gl-native/pull/14839))
* Fixed an issue where `MGLFillExtrusionStyleLayer` vertical gradients might not be rendered. ([#14808](https://github.com/mapbox/mapbox-gl-native/pull/14808))

### Other changes

* The `-[MGLMapView setCamera:withDuration:animationTimingFunction:edgePadding:completionHandler:]` method now adds the current value of the `MGLMapView.contentInset` property to the `edgePadding` parameter. ([#14813](https://github.com/mapbox/mapbox-gl-native/pull/14813))
* Setting `MGLMapView.contentInset` now moves the map’s focal point to the center of the content frame after insetting. ([#14664](https://github.com/mapbox/mapbox-gl-native/pull/14664))
* Fixed a feature querying bug caused by incorrect sort feature index calculation. ([#14884](https://github.com/mapbox/mapbox-gl-native/pull/14884))

## 5.0.0 - May 22, 2019

This release improves how monthly active users are counted. By upgrading to this release, you are opting into the changes outlined in [this blog post](https://www.mapbox.com/52219) and [#14421](https://github.com/mapbox/mapbox-gl-native/pull/14421).

There are no breaking API changes in this release.

### Styles and rendering

* Changed placement order of `MGLSymbolStyleLayer` to match the viewport-y order when `MGLSymbolStyleLayer.symbolZOrder` is set to `MGLSymbolZOrderViewportY`, allowing icons to overlap but not text. ([#14486](https://github.com/mapbox/mapbox-gl-native/pull/14486))
* Added `MGLSymbolStyleLayer.symbolSortKey` and `MGLSymbolZOrderAuto` to allow customization of symbol z-ordering. ([#14386](https://github.com/mapbox/mapbox-gl-native/pull/14386))

### Other changes

* Fixed an issue where `-[MGLMapView setVisibleCoordinates:count:edgePadding:direction:duration:animationTimingFunction:completionHandler:]` interpreted a negative `direction` as due north instead of maintaining the current direction. ([#14575](https://github.com/mapbox/mapbox-gl-native/pull/14575))

## 4.11.0 - May 15, 2019

### Styles and rendering

* Fixed a bug where some layers weren’t rendering correctly after panning. ([#14527](https://github.com/mapbox/mapbox-gl-native/pull/14527))
* Speculatively fixed a rare background crash when receiving a memory warning. ([#14383](https://github.com/mapbox/mapbox-gl-native/pull/14383))

### Annotations

* Fixed a bug with jittery callout views when using sprite-based annotations. ([#14445](https://github.com/mapbox/mapbox-gl-native/pull/14445))

### Other changes

* Fixed an Interface Builder crash when using an `MGLMapView` in a storyboard. ([#14379](https://github.com/mapbox/mapbox-gl-native/pull/14379))
* Fixed a bug that caused incorrect positioning of the attribution dialog after rotation. ([#14185](https://github.com/mapbox/mapbox-gl-native/pull/14185))
* Improved `MGLLocationManager` optional protocol properties bridging to Swift. ([#14477](https://github.com/mapbox/mapbox-gl-native/pull/14477))
* Fixed a layout constraints crash on iOS 9 when a view is removed from its superview. ([#14529](https://github.com/mapbox/mapbox-gl-native/pull/14529))

## 4.10.0 - April 17, 2019

### Styles and rendering

* Client-side text rendering of CJK ideographs is now enabled by default. ([#13988](https://github.com/mapbox/mapbox-gl-native/pull/13988))
* Fixed an issue that caused `MGL_FUNCTION` to ignore multiple formatting parameters when passed a `format` function as parameter. ([#14064](https://github.com/mapbox/mapbox-gl-native/pull/14064))
* Added `mgl_attributed:` expression operator, which concatenates `MGLAttributedExpression` objects for specifying rich text in the `MGLSymbolStyleLayer.text` property. ([#14094](https://github.com/mapbox/mapbox-gl-native/pull/14094))
* Fixed an issue that caused conditional expressions to crash when passed nested conditional expressions as parameters. ([#14181](https://github.com/mapbox/mapbox-gl-native/pull/14181))
* Added `-[MGLMapViewDelegate mapView:didFailToLoadImage:]` to load missing symbol icons in the style if they are not found. ([#14302](https://github.com/mapbox/mapbox-gl-native/pull/14302))
* Fixed a possible crash with certain expressions containing arguments that evaluate to a dictionary containing `NSArray` or `NSNumber` values. ([#14352](https://github.com/mapbox/mapbox-gl-native/pull/14352))
* Fixed a bug where non-opaque `UIColor` values were ignored when assigned to a style layer color property. ([#14406](https://github.com/mapbox/mapbox-gl-native/pull/14406))
* Speculatively fixed a bug where GL rendering could occur in the background. ([#14439](https://github.com/mapbox/mapbox-gl-native/pull/14439))

### Packaging

* Added a Galician localization. ([#14095](https://github.com/mapbox/mapbox-gl-native/pull/14095))
* Added support for building with Xcode 10.2 / iOS SDK 12.2. ([#14241](https://github.com/mapbox/mapbox-gl-native/pull/14241))

### Offline maps

* Fixed a bug that caused offline packs created prior to v4.0.0 to be marked as `MGLOfflinePackStateInactive`. ([#14188](https://github.com/mapbox/mapbox-gl-native/pull/14188))

### Other changes

* Added `MGLOrnamentPosition` enum and margin properties to customize scale bar, compass, logo, and attribution position within the map view. ([#13911](https://github.com/mapbox/mapbox-gl-native/pull/13911))
* Added an `MGLMapView.prefetchesTiles` property to configure lower-resolution tile prefetching behavior. ([#14031](https://github.com/mapbox/mapbox-gl-native/pull/14031))
* Speculatively fixed a performance issue seen on iOS 12.2, when an `MGLMapView` is repeatedly removed and re-added in a view hierarchy. ([#14264](https://github.com/mapbox/mapbox-gl-native/pull/14264))

## 4.9.0 - February 27, 2019

* Fixed a bug where setting `MGLMapView.userTrackingMode` to `MGLUserTrackingModeFollowWithHeading` or `MGLUserTrackingModeFollowWithCourse` would be ignored if the user’s location was not already available. ([#13849](https://github.com/mapbox/mapbox-gl-native/pull/13849))
* Improved tilt gesture performance. ([#13902](https://github.com/mapbox/mapbox-gl-native/pull/13902))
* Fixed a bug where `layoutSubviews` was always called on device rotation, regardless of the application's or top-most view controller's supported orientations. ([#13900](https://github.com/mapbox/mapbox-gl-native/pull/13900))
* Added `MGLNetworkConfiguration` class to customize the SDK's `NSURLSessionConfiguration` object. ([#13886](https://github.com/mapbox/mapbox-gl-native/pull/13886))

## 4.8.0 - January 30, 2019

### Styles and rendering

* Added an `MGLStyle.performsPlacementTransitions` property to control how long it takes for colliding labels to fade out. ([#13565](https://github.com/mapbox/mapbox-gl-native/pull/13565))
* Fixed a crash when casting large numbers in `NSExpression`. ([#13580](https://github.com/mapbox/mapbox-gl-native/pull/13580))
* Added the `-[MGLShapeSource leavesOfCluster:offset:limit:]`, `-[MGLShapeSource childrenOfCluster:]`, `-[MGLShapeSource zoomLevelForExpandingCluster:]` methods for inspecting a cluster in an `MGLShapeSource`s created with the `MGLShapeSourceOptionClustered` option. Feature querying now returns clusters represented by `MGLPointFeatureCluster` objects (that conform to the `MGLCluster` protocol). ([#12952](https://github.com/mapbox/mapbox-gl-native/pull/12952)

### Annotations

* Fixed a bug where the `animated` parameter to `-[MGLMapView selectAnnotation:animated:]` was being ignored. ([#13689](https://github.com/mapbox/mapbox-gl-native/pull/13689))
* Fixed a bug where selecting partially on-screen annotations (without a callout) would move the map. ([#13727](https://github.com/mapbox/mapbox-gl-native/pull/13727))

### Packaging

* Added a Czech localization. ([#13782](https://github.com/mapbox/mapbox-gl-native/pull/13782))

### Other changes

* Reinstates version 11 as the default Mapbox Streets style (as introduced in 4.7.0). ([#13690](https://github.com/mapbox/mapbox-gl-native/pull/13690))
* `MGLMapView` no longer freezes on external displays connected through AirPlay or CarPlay when the main device’s screen goes to sleep or the user manually locks the screen. ([#13701](https://github.com/mapbox/mapbox-gl-native/pull/13701))
* Fixed an issue calculating the viewport when setting visible coordinates with a `direction`. ([#13761](https://github.com/mapbox/mapbox-gl-native/pull/13761))

## 4.7.1 - December 21, 2018

### Styles and rendering

* Reverts the ability for `MGLMapView`, `MGLShapeOfflineRegion`, and `MGLTilePyramidOfflineRegion` to use version 11 of the Mapbox Streets style. ([#13650](https://github.com/mapbox/mapbox-gl-native/pull/13650))
* Reverts the ability for convenience methods on `MGLStyle` such as `MGLStyle.lightStyleURL`, to use version 11 of the Mapbox Streets style. ([#13650](https://github.com/mapbox/mapbox-gl-native/pull/13650))

## 4.7.0 - December 18, 2018

### Packaging

* Added the `Mapbox-iOS-SDK-stripped` build flavor, featuring fewer debug symbols. Regular framework binaries are no longer stripped of debug symbols and the `Mapbox-iOS-SDK-symbols` build has been retired. ([#13504](https://github.com/mapbox/mapbox-gl-native/pull/13504))
* This SDK’s dynamic framework now has a bundle identifier of `com.mapbox.Mapbox`. ([#12857](https://github.com/mapbox/mapbox-gl-native/pull/12857))
* `MGLMapView`, `MGLShapeOfflineRegion`, and `MGLTilePyramidOfflineRegion` now default to version 11 of the Mapbox Streets style. Similarly, several class properties of `MGLStyle`, such as `MGLStyle.lightStyleURL`, have been updated to return URLs to new versions of their respective styles. ([#13585](https://github.com/mapbox/mapbox-gl-native/pull/13585))

### Styles and rendering

* Fixed an issue where the `{prefix}` token in tile URL templates was evaluated incorrectly when requesting a source’s tiles. ([#13429](https://github.com/mapbox/mapbox-gl-native/pull/13429))
* Added an `-[MGLStyle removeSource:error:]` method that returns a descriptive error if the style fails to remove the source, whereas `-[MGLStyle removeSource:]` fails silently. ([#13399](https://github.com/mapbox/mapbox-gl-native/pull/13399))
* Added the `MGLFillExtrusionStyleLayer.fillExtrusionHasVerticalGradient` property. ([#13463](https://github.com/mapbox/mapbox-gl-native/pull/13463))
* Added support for setting `MGLCollisionBehaviorPre4_0` in `NSUserDefaults`. ([#13426](https://github.com/mapbox/mapbox-gl-native/pull/13426))
* `-[MGLStyle localizeLabelsIntoLocale:]` and `-[NSExpression(MGLAdditions) mgl_expressionLocalizedIntoLocale:]` can automatically localize styles that use version 8 of the Mapbox Streets source. ([#13481](https://github.com/mapbox/mapbox-gl-native/pull/13481))
* Fixed symbol flickering during instantaneous transitions. ([#13535](https://github.com/mapbox/mapbox-gl-native/pull/13535))
* Fixed a crash when specifying `MGLShapeSourceOptionLineDistanceMetrics` when creating an `MGLShapeSource`. ([#13543](https://github.com/mapbox/mapbox-gl-native/pull/13543))

### Map snapshots

* `MGLMapSnapshotter` now respects the `MGLIdeographicFontFamilyName` key in Info.plist, which reduces bandwidth consumption when snapshotting regions that contain Chinese or Japanese characters. ([#13427](https://github.com/mapbox/mapbox-gl-native/pull/13427))
* Fixed a sporadic crash when using `MGLMapSnapshotter`. ([#13300](https://github.com/mapbox/mapbox-gl-native/pull/13300))

### Other changes

* Modified the behavior of the map view so that programmatic camera transitions can no longer be interrupted by user interaction when `MGLMapView.zoomEnabled`, `MGLMapView.rotateEnabled`, `MGLMapView.scrollEnabled`, and `MGLMapView.pitchEnabled` are set to false. ([#13362](https://github.com/mapbox/mapbox-gl-native/pull/13362))
* Renamed `-[MGLOfflineStorage putResourceWithUrl:data:modified:expires:etag:mustRevalidate:]` to `-[MGLOfflineStorage preloadData:forURL:modificationDate:expirationDate:eTag:mustRevalidate:]`. ([#13318](https://github.com/mapbox/mapbox-gl-native/pull/13318))
* Points of interest have clearer, localized VoiceOver hints in styles that use version 8 of the Mapbox Streets source. ([#13525](https://github.com/mapbox/mapbox-gl-native/pull/13525))
* Added `MGLLoggingConfiguration` and `MGLLoggingBlockHandler` that handle error and fault events produced by the SDK. ([#13235](https://github.com/mapbox/mapbox-gl-native/pull/13235))
* Fixed random crashes during app termination. ([#13367](https://github.com/mapbox/mapbox-gl-native/pull/13367))

## 4.6.0 - November 7, 2018

### Styles and rendering

* `MGLSymbolStyleLayer.text` can now be set to rich text with varying fonts and text sizes. ([#12624](https://github.com/mapbox/mapbox-gl-native/pull/12624))
* Fixed a crash when using the `MGL_LET`, `MGL_MATCH`, `MGL_IF`, or `MGL_FUNCTION` functions without a colon inside an `NSExpression` or `NSPredicate` format string. ([#13189](https://github.com/mapbox/mapbox-gl-native/pull/13189))
* Fixed a crash setting the `MGLLineStyleLayer.lineGradient` property to an expression containing the `$lineProgress` variable. Added an `NSExpression.lineProgressVariableExpression` class property that returns an expression for the `$lineProgress` variable. ([#13192](https://github.com/mapbox/mapbox-gl-native/pull/13192))
* Feature querying can now return point features represented by icons that have both the `MGLSymbolStyleLayer.iconRotation` and `MGLSymbolStyleLayer.iconOffset` properties applied. ([#13105](https://github.com/mapbox/mapbox-gl-native/pull/13105))
* Fixed an issue where polygons crossing tile boundaries could be improperly clipped. ([#13231](https://github.com/mapbox/mapbox-gl-native/pull/13231))

### Offline maps

* Network requests by `MGLMapView` are now prioritized over offline pack downloads. ([#13019](https://github.com/mapbox/mapbox-gl-native/pull/13019))
* Added the `-[MGLOfflineStorage putResourceWithUrl:data:modified:expires:etag:mustRevalidate:]` method to allow pre-warming of the ambient cache. ([#13119](https://github.com/mapbox/mapbox-gl-native/pull/13119))

### Other changes

* Fixed an issue where the map view could not be panned after setting `MGLMapView.visibleCoordinateBounds` to a coordinate bounds that spanned exactly the longitudes −180° and 180°. ([#13006](https://github.com/mapbox/mapbox-gl-native/pull/13006))
* Fixed an issue where snapshots had the wrong heading and pitch. ([#13123](https://github.com/mapbox/mapbox-gl-native/pull/13123))
* Fixed an issue where `-[MGLMapViewDelegate mapView:shouldChangeFromCamera:toCamera:]` was called with an incorrectly rotated `newCamera` when the user rotated the map. ([#13123](https://github.com/mapbox/mapbox-gl-native/pull/13123))

## 4.5.0 - October 10, 2018

### Styles and rendering

* Added support for 120 frames per second on capable devices. ([#12979](https://github.com/mapbox/mapbox-gl-native/pull/12979))
* Added an `MGLSymbolStyleLayer.symbolZOrder` property for forcing point features in a symbol layer to be layered in the same order that they are specified in the layer’s associated source. ([#12783](https://github.com/mapbox/mapbox-gl-native/pull/12783))
* Fixed a crash when the `MGLBackgroundStyleLayer.backgroundPattern`, `MGLFillExtrusionStyleLayer.fillExtrusionPattern`, `MGLFillStyleLayer.fillPattern`, or `MGLLineStyleLayer.linePattern` property evaluates to `nil` for a particular feature. ([#12896](https://github.com/mapbox/mapbox-gl-native/pull/12896))
* Fixed an issue with view annotations (including the user location annotation) and the GL map lagging relative to each other. ([#12895](https://github.com/mapbox/mapbox-gl-native/pull/12895))
* Fixed an issue where features in `MGLFillStyleLayer` and `MGLLineStyleLayer` would occasionally flicker when zooming in and out. ([#12982](https://github.com/mapbox/mapbox-gl-native/pull/12982))
* Fixed a crash when casting a `UIColor` to a `UIColor` inside an `NSExpression`. ([#12864](https://github.com/mapbox/mapbox-gl-native/pull/12864))
* `NIL` cast to an `NSNumber` now evaluates to 0 inside an `NSExpression`. ([#12864](https://github.com/mapbox/mapbox-gl-native/pull/12864))
* Fixed a crash when applying the `to-array` operator to an empty array inside a JSON expression. ([#12864](https://github.com/mapbox/mapbox-gl-native/pull/12864))
* Added the `MGLCollisionBehaviorPre4_0` Info.plist key to restore the collision detection behavior in version 3.7 of the SDK. ([#12941](https://github.com/mapbox/mapbox-gl-native/pull/12941))

### User location

* Added `-[MGLMapViewDelegate mapViewUserLocationAnchorPoint:]` to customize the position of the user location annotation. ([#12907](https://github.com/mapbox/mapbox-gl-native/pull/12907))
* Marked `MGLMapView.userLocationVerticalAlignment` as deprecated. Use `-[MGLMapViewDelegate mapViewUserLocationAnchorPoint:]` instead. ([#12907](https://github.com/mapbox/mapbox-gl-native/pull/12907))
* Added the `-[MGLMapView updateUserLocationAnnotationView]` and `-[MGLMapView updateUserLocationAnnotationViewAnimatedWithDuration:]` methods to update the position of the user location annotation between location updates. ([#12907](https://github.com/mapbox/mapbox-gl-native/pull/12907))
* Fixed an issue where the user location annotation was positioned incorrectly when the map view had a left or right content inset. ([#12907](https://github.com/mapbox/mapbox-gl-native/pull/12907))

### Offline maps

* Added `-[MGLOfflineStorage addContentsOfFile:withCompletionHandler:]` and `-[MGLOfflineStorage addContentsOfURL:withCompletionHandler:]` methods to add pregenerated offline packs to offline storage. ([#12791](https://github.com/mapbox/mapbox-gl-native/pull/12791))
* Fixed an issue where some tiles were rendered incorrectly when the device was unable to connect to the Internet. ([#12931](https://github.com/mapbox/mapbox-gl-native/pull/12931))

### Other changes

* Added `MGLAltitudeForZoomLevel()` and `MGLZoomLevelForAltitude()` methods for converting between zoom levels used by `MGLMapView` and altitudes used by `MGLMapCamera`. ([#12986](https://github.com/mapbox/mapbox-gl-native/pull/12986))
* Deprecated the `+[MGLMapCamera cameraLookingAtCenterCoordinate:fromDistance:pitch:heading:]` method in favor of `+[MGLMapCamera cameraLookingAtCenterCoordinate:altitude:pitch:heading:]` and `+[MGLMapCamera cameraLookingAtCenterCoordinate:acrossDistance:pitch:heading:]`. ([#12966](https://github.com/mapbox/mapbox-gl-native/pull/12966))
* Fixed an issue where `+[MGLMapCamera cameraLookingAtCenterCoordinate:fromEyeCoordinate:eyeAltitude:]` created a camera looking from the wrong eye coordinate. ([#12966](https://github.com/mapbox/mapbox-gl-native/pull/12966))
* Added an `MGLMapCamera.viewingDistance` property based on the existing `MGLMapCamera.altitude` property. ([#12966](https://github.com/mapbox/mapbox-gl-native/pull/12966))
* Fixed an issue where `-[MGLMapSnapshotter startWithQueue:completionHandler:]` failed to call its completion handler in some cases. ([#12355](https://github.com/mapbox/mapbox-gl-native/pull/12355))
* Fixed an issue where symbols from the events library could be duplicated when the maps SDK was used in conjunction with another Mapbox framework. ([#13008](https://github.com/mapbox/mapbox-gl-native/pull/13008))

## 4.4.1 - September 13, 2018

* Fixed several crashes related to telemetry collection. ([#12825](https://github.com/mapbox/mapbox-gl-native/pull/12825))
* Fixed a crash when the network connection was offline. ([#12889](https://github.com/mapbox/mapbox-gl-native/pull/12889))

## 4.4.0 - September 12, 2018

### Styles and rendering

* When a symbol in an `MGLSymbolStyleLayer` has both an icon and text, both are shown or hidden together based on available space. ([#12521](https://github.com/mapbox/mapbox-gl-native/pull/12521))
* Invalid values of `MGLSymbolStyleLayer.textFontNames` are treated as warnings instead of errors. ([#12414](https://github.com/mapbox/mapbox-gl-native/pull/12414))
* Added an `MGLLineStyleLayer.lineGradient` property that can be used to define a gradient with which to color a line feature. ([#12575](https://github.com/mapbox/mapbox-gl-native/pull/12575))
* The `MGLLineStyleLayer.linePattern`, `MGLFillStyleLayer.fillPattern`, and `MGLFillExtrusionStyleLayer.fillExtrusionPattern` properties can now be set to expressions that refer to feature attributes. ([#12284](https://github.com/mapbox/mapbox-gl-native/pull/12284))
* Reduced the amount of memory consumed by font data after changing the style. ([#12414](https://github.com/mapbox/mapbox-gl-native/pull/12414))
* `-[MGLShapeSource initWithIdentifier:shape:options:]` warns about possible attribute loss when passing in an `MGLShapeCollection` object. ([#12625](https://github.com/mapbox/mapbox-gl-native/pull/12625))
* Added an `MGLShapeSourceOptionLineDistanceMetrics` option that enables or disables calculating line distance metrics. ([#12604](https://github.com/mapbox/mapbox-gl-native/pull/12604))
* Fixed an issue where the `cubic-bezier` curve type for `mgl_interpolate:withCurveType:parameters:stops:` expressions was misinterpreted for some style layer properties. ([#12826](https://github.com/mapbox/mapbox-gl-native/pull/12826))
* Fixed an issue that could cause symbols to fade in during pan operations instead of always showing when using `MGLSymbolStyleLayer.iconAllowsOverlap` or `MGLSymbolStyleLayer.textAllowsOverlap` properties. ([#12698](https://github.com/mapbox/mapbox-gl-native/pull/12698))

### Offline maps

* Added the `MGLShapeOfflineRegion` class for creating an offline pack that covers an arbitrary shape. ([#11447](https://github.com/mapbox/mapbox-gl-native/pull/11447))
* Fixed crashes when offline storage encountered certain SQLite errors. ([#12224](https://github.com/mapbox/mapbox-gl-native/pull/12224))

### Other changes

* The predefined values of `MGLMapView.decelerationRate` are now typed as `MGLMapViewDecelerationRate`s for improved bridging to Swift. ([#12584](https://github.com/mapbox/mapbox-gl-native/pull/12584))
* Added an `-[MGLMapViewDelegate mapView:shapeAnnotationIsEnabled:]` method to specify whether an annotation is selectable. ([#12352](https://github.com/mapbox/mapbox-gl-native/pull/12352))
* The `-[MGLMapView visibleFeaturesAtPoint:]` method can now return features near tile boundaries at high zoom levels. ([#12570](https://github.com/mapbox/mapbox-gl-native/pull/12570))
* Fixed inconsistencies in exception naming. ([#12583](https://github.com/mapbox/mapbox-gl-native/issues/12583))
* Fixed an issue where `-[MGLMapView convertCoordinateBounds:toRectToView:]` would return an empty CGRect if the bounds crossed the antimeridian. ([#12758](https://github.com/mapbox/mapbox-gl-native/pull/12758))

## 4.3.0 - August 15, 2018

### Styles and rendering

* Added an `MGLMapView.preferredFramesPerSecond` property that controls the rate at which the map view is rendered. The default rate now adapts to device capabilities to provide a smoother experience. ([#12501](https://github.com/mapbox/mapbox-gl-native/issues/12501))
* Token string syntax (`"{token}"`) in `MGLSymbolStyleLayer` `text` and `iconImageName` properties is now correctly converted to the appropriate `NSExpression` equivalent. ([#11659](https://github.com/mapbox/mapbox-gl-native/issues/11659))
* Fixed a crash when switching between two styles having layers with the same identifier but different layer types. ([#12432](https://github.com/mapbox/mapbox-gl-native/issues/12432))
* Added a new option to `MGLSymbolPlacement`, `MGLSymbolPlacementLineCenter`, that places the label relative to the center of the geometry. ([#12337](https://github.com/mapbox/mapbox-gl-native/pull/12337))

### User location

* Added an `MGLMapView.locationManager` property and `MGLLocationManager` protocol for tracking user location using a custom alternative to `CLLocationManager`. ([#12013](https://github.com/mapbox/mapbox-gl-native/pull/12013))
* Fixed a crash that occurred when `MMELocationManager` was deallocated and the delegate was reporting updates. ([#12542](https://github.com/mapbox/mapbox-gl-native/pull/12542))

### Other changes

* Fixed a crash that occurred when the user started a gesture before the drift animation for a previous gesture was complete. ([#12148](https://github.com/mapbox/mapbox-gl-native/pull/12148))
* Fixed an issue where the symbols for `MGLMapPointForCoordinate` could not be found. ([#12445](https://github.com/mapbox/mapbox-gl-native/issues/12445))
* Fixed an issue causing country and ocean labels to disappear after calling `-[MGLStyle localizeLabelsIntoLocale:]` when the system language is set to Simplified Chinese. ([#12164](https://github.com/mapbox/mapbox-gl-native/issues/12164))
* Closed a security vulnerability introduced in 4.1.0 that would potentially allow the owner of a style to compromise apps loading that style. ([#12571](https://github.com/mapbox/mapbox-gl-native/pull/12571))
* Reduced binary size and improved performance by enabling LTO. ([#12502](https://github.com/mapbox/mapbox-gl-native/pull/12502))

## 4.0.5 - August 15, 2018

### Packaging

* When integrating this framework using CocoaPods, the included bcsymbolmap files are now preserved. If you have bitcode enabled and you are seeing incorrectly symbolicated crash logs, you should create a build phase in your Xcode project that copies these bcsymbolmap files to your app’s Products Directory when installing. ([#12257](https://github.com/mapbox/mapbox-gl-native/pull/12257))

### Other changes

* Added an `MGLMapView.locationManager` property and `MGLLocationManager` protocol for tracking user location using a custom alternative to `CLLocationManager`. ([#12013](https://github.com/mapbox/mapbox-gl-native/pull/12013))

## 4.2.0 - July 18, 2018

### Packaging

* When integrating this framework using CocoaPods, the included bcsymbolmap files are now preserved. If you have bitcode enabled and you are seeing incorrectly symbolicated crash logs, you should create a build phase in your Xcode project that copies these bcsymbolmap files to your app’s Products Directory when installing. ([#12257](https://github.com/mapbox/mapbox-gl-native/pull/12257))

### Styles and rendering

* Added an `MGLRasterStyleLayer.rasterResamplingMode` property for configuring how raster style layers are overscaled. ([#12176](https://github.com/mapbox/mapbox-gl-native/pull/12176))
* `-[MGLStyle localizeLabelsIntoLocale:]` and `-[NSExpression mgl_expressionLocalizedIntoLocale:]` can automatically localize labels into Japanese or Korean based on the system’s language settings. ([#12286](https://github.com/mapbox/mapbox-gl-native/pull/12286))
* The `c` and `d` options are supported within comparison predicates for case and diacritic insensitivity, respectively. ([#12329](https://github.com/mapbox/mapbox-gl-native/pull/12329))
* Added the `collator` and `resolved-locale` expression operators to more precisely compare strings in style JSON. A subset of this functionality is available through predicate options when creating an `NSPredicate`. ([#11869](https://github.com/mapbox/mapbox-gl-native/pull/11869))
* Fixed a crash when trying to parse expressions containing legacy filters. ([#12263](https://github.com/mapbox/mapbox-gl-native/pull/12263))
* Fixed a crash that occurred when creating an `MGL_MATCH` expression using non-expressions as arguments. ([#12332](https://github.com/mapbox/mapbox-gl-native/pull/12332))

### Networking and storage

* Improved caching performance. ([#12072](https://github.com/mapbox/mapbox-gl-native/pull/12072))

### Other changes

* Added `-[MGLMapView camera:fittingShape:edgePadding:]` and `-[MGLMapView camera:fittingCoordinateBounds:edgePadding:]` allowing you specify the pitch and direction for the calculated camera. ([#12213](https://github.com/mapbox/mapbox-gl-native/pull/12213))
* Added `-[MGLMapSnapshot coordinateForPoint:]` that returns a map coordinate for a specified snapshot image point. ([#12221](https://github.com/mapbox/mapbox-gl-native/pull/12221))
* Reduced memory usage when collision debug mode is disabled. ([#12294](https://github.com/mapbox/mapbox-gl-native/issues/12294))
* Fixed a bug with annotation view touch handling when a non-zero `centerOffset` is specified. ([#12234](https://github.com/mapbox/mapbox-gl-native/pull/12234))

## 4.0.4 - June 27, 2018

* Improved compatibility with Mapbox China APIs. ([#12233](https://github.com/mapbox/mapbox-gl-native/pull/12233))

## 4.0.3 - June 22, 2018

* Fixed a crash in `-[MGLStyle localizeLabelsIntoLocale:]` on iOS 9._x_. ([#12123](https://github.com/mapbox/mapbox-gl-native/pull/12123))
* Improved compatibility with Mapbox China APIs. ([#11845](https://github.com/mapbox/mapbox-gl-native/pull/11845))

## 4.1.0 - June 20, 2018

### Packaging

* The minimum deployment target for this SDK is now iOS 9.0. ([#11776](https://github.com/mapbox/mapbox-gl-native/pull/11776))
* Removed support for the Fabric distribution platform. ([#12106](https://github.com/mapbox/mapbox-gl-native/pull/12106))
* Improved compatibility with Mapbox China APIs. ([#11845](https://github.com/mapbox/mapbox-gl-native/pull/11845))

### Styles and rendering

* Added support for aggregate expressions as input values to `MGL_MATCH` expressions. ([#11866](https://github.com/mapbox/mapbox-gl-native/pull/11866))
* Fixed a crash that occurred when style JSON contained an invalid filter containing an expression. ([#12065](https://github.com/mapbox/mapbox-gl-native/pull/12065))
* Fixed a crash in `-[MGLStyle localizeLabelsIntoLocale:]` on iOS 9._x_. ([#12123](https://github.com/mapbox/mapbox-gl-native/pull/12123))
* Unknown tokens in URLs are now preserved, rather than replaced with an empty string. ([#11787](https://github.com/mapbox/mapbox-gl-native/issues/11787))
* Fixed an issue preventing nested key path expressions from accessing the correct feature attributes. ([#11959](https://github.com/mapbox/mapbox-gl-native/pull/11959))
* Fixed an issue where `MGLSymbolStyleLayer` flickered when straddling the antimeridian. ([#11938](https://github.com/mapbox/mapbox-gl-native/pull/11938))
* Fixed an issue where certain `MGLLineStyleLayer.lineDashPattern` values produced unexpected rendering. ([#12114](https://github.com/mapbox/mapbox-gl-native/pull/12114))

### Other changes

* Adjusted when and how the camera transition update and finish callbacks are called, fixing recursion bugs. ([#11614](https://github.com/mapbox/mapbox-gl-native/pull/11614))
* Fixed an issue where `-[MGLMapViewDelegate mapView:tapOnCalloutForAnnotation:]` was called when the user tapped on transparent areas beneath the standard callout view. ([#11939](https://github.com/mapbox/mapbox-gl-native/pull/11939))
* Improved `MGLMapView`’s performance when the scale bar is shown. ([#11921](https://github.com/mapbox/mapbox-gl-native/pull/11921))
* Fixed a crash that could occur when reusing `MGLMapSnapshotter` or using multiple snapshotters at the same time. ([#11831](https://github.com/mapbox/mapbox-gl-native/pull/11831))
* Fixed an issue where an empty `MGLFeature` array caused high CPU utilization. ([#11985](https://github.com/mapbox/mapbox-gl-native/pull/11985))
* Improved offline download performance. ([#11284](https://github.com/mapbox/mapbox-gl-native/pull/11284))
* Fixed an issue that caused `-[MGLMapView visibleFeaturesAtPoint:]` to return an empty array when adding or removing features. ([#12076](https://github.com/mapbox/mapbox-gl-native/pull/12076))
* Improved application launch performance. ([#11784](https://github.com/mapbox/mapbox-gl-native/pull/11784))

## 4.0.2 - May 29, 2018

* Fixed a crash when constant expressions were used for style properties that didn't support data-driven styling. ([#11960](https://github.com/mapbox/mapbox-gl-native/issues/11960))
* Improved symbol querying. ([#11571](https://github.com/mapbox/mapbox-gl-native/pull/11571), [#11742](https://github.com/mapbox/mapbox-gl-native/pull/11742))

## 4.0.1 - May 14, 2018

### Packaging

* Re-added support for 32-bit simulators (i386) to work around an issue in CocoaPods. ([#11891](https://github.com/mapbox/mapbox-gl-native/pull/11891))
* Added a Korean localization. ([#11792](https://github.com/mapbox/mapbox-gl-native/pull/11792))

### Style layers

* Deprecated `+[NSExpression featurePropertiesVariableExpression]`; use `+[NSExpression featureAttributesVariableExpression]` instead. ([#11748](https://github.com/mapbox/mapbox-gl-native/pull/11748))
* Added an `-[NSPredicate(MGLAdditions) predicateWithMGLJSONObject:]` method and `NSPredicate.mgl_jsonExpressionObject` property. ([#11810](https://github.com/mapbox/mapbox-gl-native/pull/11810))
* Added `FIRST`, `LAST`, and `SIZE` symbolic array subscripting support to expressions. ([#11770](https://github.com/mapbox/mapbox-gl-native/pull/11770))
* Inside an expression, casting `nil` to a string turns it into the empty string instead of the string `"null"`. ([#11904](https://github.com/mapbox/mapbox-gl-native/pull/11904))
* Fixed an issue where certain colors were being misrepresented in `NSExpression` obtained from `MGLStyleLayer` getters. ([#11725](https://github.com/mapbox/mapbox-gl-native/pull/11725))

### Annotations

* Fixed an issue where selecting an onscreen annotation could move the map unintentionally. ([#11731](https://github.com/mapbox/mapbox-gl-native/pull/11731))
* Fixed an issue where annotation views could become distorted if `rotatesToMatchCamera` was enabled. ([#11817](https://github.com/mapbox/mapbox-gl-native/pull/11817))
* Fixed `MGLAnnotationView.rotatesToMatchCamera` overriding other transforms that might be applied to annotation views that had this property enabled. ([#11842](https://github.com/mapbox/mapbox-gl-native/pull/11842))
* Fixed an issue where an `MGLOverlay` object straddling the antimeridian had an empty `MGLOverlay.overlayBounds` value. ([#11783](https://github.com/mapbox/mapbox-gl-native/pull/11783))

### Other changes

* If English is the first language listed in the user’s Preferred Languages setting, `-[MGLStyle localizeLabelsIntoLocale:]` no longer prioritizes other languages over English. ([#11907](https://github.com/mapbox/mapbox-gl-native/pull/11907))
* Fixed an issue where `-[MGLMapView metersPerPixelAtLatitude:]` was removed, but not marked as unavailable. ([#11765](https://github.com/mapbox/mapbox-gl-native/pull/11765))
* Reduced per-frame render CPU time. ([#11811](https://github.com/mapbox/mapbox-gl-native/issues/11811))

## 3.7.8 - May 7, 2018

* Improved compatibility with Mapbox China APIs. ([#11845](https://github.com/mapbox/mapbox-gl-native/pull/11845))

## 3.7.7 - May 3, 2018

* Fixed a crash when removing an `MGLOfflinePack`. ([#11821](https://github.com/mapbox/mapbox-gl-native/issues/11821))

## 4.0.0 - April 19, 2018

The 4.0._x_ series of releases will be the last to support iOS 8. The minimum iOS deployment version will increase to iOS 9.0 in a future release.

### Packaging

* Removed support for 32-bit simulators. ([#10962](https://github.com/mapbox/mapbox-gl-native/pull/10962))
* Added Danish, Hebrew, and European Portuguese localizations. ([#10967](https://github.com/mapbox/mapbox-gl-native/pull/10967), [#11136](https://github.com/mapbox/mapbox-gl-native/pull/11134), [#11695](https://github.com/mapbox/mapbox-gl-native/pull/11695))
* Removed methods, properties, and constants that had been deprecated as of v3.7.6. ([#11205](https://github.com/mapbox/mapbox-gl-native/pull/11205), [#11681](https://github.com/mapbox/mapbox-gl-native/pull/11681))
* Refined certain Swift interfaces by converting them from class methods to class properties. ([#11674](https://github.com/mapbox/mapbox-gl-native/pull/11674))
* Revamped the “Adding Points to a Map” guide. ([#11496](https://github.com/mapbox/mapbox-gl-native/pull/11496))

### Style layers

* The layout and paint properties on subclasses of `MGLStyleLayer` are now of type `NSExpression` instead of `MGLStyleValue`. A new “Predicates and Expressions” guide provides an overview of the supported operators, which include arithmetic and conditional operators. ([#10726](https://github.com/mapbox/mapbox-gl-native/pull/10726))
* A style can now display a heatmap layer that visualizes a point data distribution. You can customize the appearance at runtime using the `MGLHeatmapStyleLayer` class. ([#11046](https://github.com/mapbox/mapbox-gl-native/pull/11046))
* A style can now display a smooth hillshading layer and customize its appearance at runtime using the `MGLHillshadeStyleLayer` class. Hillshading is based on a rasterized digital elevation model supplied by the `MGLRasterDEMSource` class. ([#10642](https://github.com/mapbox/mapbox-gl-native/pull/10642))
* You can now set the `MGLVectorStyleLayer.predicate` property to a predicate that contains arithmetic and calls to built-in `NSExpression` functions. You may need to cast a feature attribute key to `NSString` or `NSNumber` before comparing it to a string or number. ([#11587](https://github.com/mapbox/mapbox-gl-native/pull/11587))
* Replaced the `MGLStyle.localizesLabels` property with an `-[MGLStyle localizeLabelsIntoLocale:]` method that allows you to specify the language to localize into. Also added an `-[NSExpression(MGLAdditions) mgl_expressionLocalizedIntoLocale:]` method for localizing an individual value used with `MGLSymbolStyleLayer.text`. ([#11651](https://github.com/mapbox/mapbox-gl-native/pull/11651))
* The `MGLSymbolStyleLayer.textFontNames` property can now depend on a feature’s attributes. ([#10850](https://github.com/mapbox/mapbox-gl-native/pull/10850))
* Changes to the `MGLStyleLayer.minimumZoomLevel` and `MGLStyleLayer.maximumZoomLevel` properties take effect immediately. ([#11399](https://github.com/mapbox/mapbox-gl-native/pull/11399))

### Content sources

* Renamed `MGLRasterSource` to `MGLRasterTileSource` and `MGLVectorSource` to `MGLVectorTileSource`. ([#11568](https://github.com/mapbox/mapbox-gl-native/pull/11568))
* Added an `MGLComputedShapeSource` class that allows applications to supply vector data to a style layer on a per-tile basis. ([#9983](https://github.com/mapbox/mapbox-gl-native/pull/9983))
* Properties such as `MGLSymbolStyleLayer.iconAllowsOverlap` and `MGLSymbolStyleLayer.iconIgnoresPlacement` now account for symbols in other sources. ([#10436](https://github.com/mapbox/mapbox-gl-native/pull/10436))

### Map rendering

* Improved the reliability of collision detection between symbols near the edges of tiles, as well as between symbols when the map is tilted. It is no longer necessary to enable `MGLSymbolStyleLayer.symbolAvoidsEdges` to prevent symbols in adjacent tiles from overlapping with each other. ([#10436](https://github.com/mapbox/mapbox-gl-native/pull/10436))
* Symbols can fade in and out as the map pans, rotates, or tilts. ([#10436](https://github.com/mapbox/mapbox-gl-native/pull/10436))
* Fixed an issue preventing a dynamically-added `MGLRasterStyleLayer` from drawing until the map pans. ([#10270](https://github.com/mapbox/mapbox-gl-native/pull/10270))
* Fixed an issue preventing `MGLImageSource`s from drawing on the map when the map is zoomed in and tilted. ([#10677](https://github.com/mapbox/mapbox-gl-native/pull/10677))
* Improved the sharpness of raster tiles on Retina displays. ([#10984](https://github.com/mapbox/mapbox-gl-native/pull/10984))
* Fixed a crash parsing a malformed style. ([#11001](https://github.com/mapbox/mapbox-gl-native/pull/11001))
* Reduced memory usage by clearing in-memory tile cache before entering background. ([#11197](https://github.com/mapbox/mapbox-gl-native/pull/11197))
* Fixed an issue where symbols with empty labels would always be hidden. ([#11206](https://github.com/mapbox/mapbox-gl-native/pull/11206))
* Fixed an issue where a tilted map could flicker while displaying rotating symbols. ([#11488](https://github.com/mapbox/mapbox-gl-native/pull/11488))
* Increased the maximum width of labels by a factor of two. ([#11508](https://github.com/mapbox/mapbox-gl-native/pull/11508))

### Annotations

* Changed the default value of `MGLAnnotationView.scalesWithViewingDistance` to `NO`, to improve performance. If your use case involves many annotation views, consider keeping this property disabled. ([#11636](https://github.com/mapbox/mapbox-gl-native/pull/11636))
* Fixed an issue preventing `MGLAnnotationImage.image` from being updated. ([#10372](https://github.com/mapbox/mapbox-gl-native/pull/10372))
* Improved performance of `MGLAnnotationView`-backed annotations that have `scalesWithViewingDistance` enabled. ([#10951](https://github.com/mapbox/mapbox-gl-native/pull/10951))
* Fixed an issue where tapping a group of annotations may not have selected the nearest annotation. ([#11438](https://github.com/mapbox/mapbox-gl-native/pull/11438))
* The `MGLMapView.selectedAnnotations` property (backed by `-[MGLMapView setSelectedAnnotations:]`) now selects annotations that are off-screen. ([#9790](https://github.com/mapbox/mapbox-gl-native/issues/9790))
* The `animated` parameter to `-[MGLMapView selectAnnotation:animated:]` now controls whether the annotation and its callout are brought on-screen. If `animated` is `NO` then the annotation is selected if offscreen, but the map is not panned. Currently only point annotations are supported. Setting the `MGLMapView.selectedAnnotations` property now animates. ([#3249](https://github.com/mapbox/mapbox-gl-native/issues/3249))
* Fixed a crash when rapidly adding and removing annotations. ([#11551](https://github.com/mapbox/mapbox-gl-native/issues/11551), [#11575](https://github.com/mapbox/mapbox-gl-native/issues/11575))
* Marked protocol method `-[MGLCalloutView presentCalloutFromRect:inView:constrainedToView:animated:]` as unavailable. Use `-[MGLCalloutView presentCalloutFromRect:inView:constrainedToRect:animated:]` instead. ([#11738](https://github.com/mapbox/mapbox-gl-native/pull/11738))

### Map snapshots

* Fixed a memory leak that occurred when creating a map snapshot. ([#10585](https://github.com/mapbox/mapbox-gl-native/pull/10585))

### Other changes

* The `-[MGLMapView convertRect:toCoordinateBoundsFromView:]` method and the `MGLMapView.visibleCoordinateBounds` property’s getter now indicate that the coordinate bounds straddles the antimeridian by extending one side beyond ±180 degrees longitude. ([#11265](https://github.com/mapbox/mapbox-gl-native/pull/11265))
* Feature querying results now account for the `MGLSymbolStyleLayer.circleStrokeWidth` property. ([#10897](https://github.com/mapbox/mapbox-gl-native/pull/10897))
* Fixed an issue preventing labels from being transliterated when VoiceOver was enabled on iOS 10._x_ and below. ([#10881](https://github.com/mapbox/mapbox-gl-native/pull/10881))
* Labels are now transliterated from more languages when VoiceOver is enabled. ([#10881](https://github.com/mapbox/mapbox-gl-native/pull/10881))
* Long-pressing the attribution button causes the SDK’s version number to be displayed in the action sheet that appears. ([#10650](https://github.com/mapbox/mapbox-gl-native/pull/10650))
* Reduced offline download sizes for styles with symbol layers that render only icons, and no text. ([#11055](https://github.com/mapbox/mapbox-gl-native/pull/11055))
* Added haptic feedback that occurs when the user rotates the map to due north, configurable via `MGLMapView.hapticFeedbackEnabled`. ([#10847](https://github.com/mapbox/mapbox-gl-native/pull/10847))
* Added `MGLMapView.showsScale` as the recommended way to show the scale bar. This property can be set directly in Interface Builder. ([#11335](https://github.com/mapbox/mapbox-gl-native/pull/11335))
* Fixed an issue where the scale bar would not appear until the map had moved. ([#11335](https://github.com/mapbox/mapbox-gl-native/pull/11335))

## 3.7.6 - March 12, 2018

* Fixed an issue where full-resolution tiles could fail to replace lower-resolution placeholders. ([#11227](https://github.com/mapbox/mapbox-gl-native/pull/11227))
* Fixed an issue where tilesets with bounds that cover the entire world would fail to render. ([#11425](https://github.com/mapbox/mapbox-gl-native/pull/11425))
* Fixed a memory leak in `MGLMapSnapshotter`. ([#11193](https://github.com/mapbox/mapbox-gl-native/pull/11193))
* Fixed an issue where the pinch gesture could drift beyond bounds imposed by `-[MGLMapViewDelegate mapView:shouldChangeFromCamera:toCamera:]`. ([#11423](https://github.com/mapbox/mapbox-gl-native/pull/11423))
* Improved the visibility of the heading indicator arrow. ([#11337](https://github.com/mapbox/mapbox-gl-native/pull/11337))

## 3.7.5 - February 16, 2018

* Fixed an issue where requesting location services permission would trigger an unrecoverable loop. ([#11229](https://github.com/mapbox/mapbox-gl-native/pull/11229))

## 3.7.4 - February 12, 2018

* Added the `MGLTileSourceOptionTileCoordinateBounds` option to create an `MGLTileSource` that only supplies tiles within a specific geographic bounding box. ([#11141](https://github.com/mapbox/mapbox-gl-native/pull/11141))
* Fixed an issue that caused `-[MGLMapSnapshotter pointForCoordinate:]` to return the wrong point. ([#11035](https://github.com/mapbox/mapbox-gl-native/pull/11035))

## 3.7.3 - January 10, 2018

* Fixed a crash while zooming while annotations are present on the map. ([#10791](https://github.com/mapbox/mapbox-gl-native/pull/10791))
* CJK characters can be displayed in a locally installed font or a custom font bundled with the application, reducing map download times. Specify the font name using the `MGLIdeographicFontFamilyName` key in the application’s Info.plist file. ([#10522](https://github.com/mapbox/mapbox-gl-native/pull/10522))
* Fixed a hang that could occur if the application makes many changes to user defaults immediately after launching. ([#10803](https://github.com/mapbox/mapbox-gl-native/pull/10803))

## 3.7.2 - December 21, 2017

### Packaging

* Reduced the file size of the dSYM by removing the i386 architecture. Support for the i386 architecture (used by 32-bit simulators) will also be removed from the framework itself in a future release. ([#10781](https://github.com/mapbox/mapbox-gl-native/pull/10781))

### Other changes

* Fixed an issue where removing a `MGLOpenGLStyleLayer` from a map might result in a crash. ([#10765](https://github.com/mapbox/mapbox-gl-native/pull/10765))
* Added documentation for usage of coordinate bounds that cross the anti-meridian. ([#9804](https://github.com/mapbox/mapbox-gl-native/issues/9804))
* Removed duplicated variables in `MGLMapSnapshotter`. ([#10702](https://github.com/mapbox/mapbox-gl-native/pull/10702))

## 3.7.1 - December 6, 2017

### Packaging

* Renamed this SDK from Mapbox iOS SDK to Mapbox Maps SDK for iOS. ([#10610](https://github.com/mapbox/mapbox-gl-native/pull/10610))

### Annotations

* Fixed incorrect hit targets for `MGLAnnotationImage`-backed annotations that caused `-[MGLMapViewDelegate mapView:didSelectAnnotation:]` to be called unnecessarily. ([#10538](https://github.com/mapbox/mapbox-gl-native/pull/10538))

### Other changes

* Fixed an issue that caused  `MGLMapView.minimumZoomLevel` to not be set. ([#10596](https://github.com/mapbox/mapbox-gl-native/pull/10596))

## 3.7.0 - Novemeber 13, 2017

### Networking and storage

* Added a new `MGLMapSnapshotter` class for capturing rendered map images from an `MGLMapView`’s camera. ([#9891](https://github.com/mapbox/mapbox-gl-native/pull/9891))
* Reduced the time it takes to create new `MGLMapView` instances in some cases. ([#9864](https://github.com/mapbox/mapbox-gl-native/pull/9864))
* Added support for forced cache revalidation that will eliminate flickering that was sometimes visible for certain types of tiles (e.g., traffic tiles). ([#9670](https://github.com/mapbox/mapbox-gl-native/pull/9670), [#9103](https://github.com/mapbox/mapbox-gl-native/issues/9103))
* Improved the performance of the SDK when parsing vector tile data used to render the map. ([#9312](https://github.com/mapbox/mapbox-gl-native/pull/9312))

### Styles

* Added a new type of source, represented by the `MGLImageSource` class at runtime, that displays a georeferenced image. ([#9110](https://github.com/mapbox/mapbox-gl-native/pull/9110))
* Setting a style using `MGLMapView`'s `styleURL` property now smoothly transitions from the previous style to the new style and maintains equivalent layers and sources along with their identifiers. ([#9256](https://github.com/mapbox/mapbox-gl-native/pull/9256))
* Added `MGLCircleStyleLayer.circlePitchAlignment` and `MGLSymbolStyleLayer.iconPitchAlignment` properties to control whether circles and symbols lie flat against a tilted map. ([#9426](https://github.com/mapbox/mapbox-gl-native/pull/9426), [#9479](https://github.com/mapbox/mapbox-gl-native/pull/9479))
* Added an `MGLSymbolStyleLayer.iconAnchor` property to control where an icon is anchored. ([#9849](https://github.com/mapbox/mapbox-gl-native/pull/9849))
* The `maximumTextWidth` and `textLetterSpacing` properties of `MGLSymbolStyleLayer` are now compatible with `MGLSourceStyleFunction`s and `MGLCompositeStyleFunction`s, allowing data-driven styling of these properties. ([#9870](https://github.com/mapbox/mapbox-gl-native/pull/9870))
* The `MGLSymbolStyleLayer.textAnchor`, `MGLSymbolStyleLayer.textJustification` and `MGLLineStyleLayer.lineJoin` properties are now compatible with `MGLSourceStyleFunction`s and `MGLCompositeStyleFunction`s, allowing data-driven styling of these properties. ([#9583](https://github.com/mapbox/mapbox-gl-native/pull/9583))
* Improved the legibility of labels that follow lines when the map is tilted. ([#9009](https://github.com/mapbox/mapbox-gl-native/pull/9009))
* Fixed an issue that could cause flickering when a translucent raster style layer was present. ([#9468](https://github.com/mapbox/mapbox-gl-native/pull/9468))
* Fixed an issue that could cause antialiasing between polygons on the same layer to fail if the fill layers used data-driven styling for the fill color. ([#9699](https://github.com/mapbox/mapbox-gl-native/pull/9699))
* The previously deprecated support for style classes has been removed. For interface compatibility, the API methods remain, but they are now non-functional.

### Annotations

* Fixed several bugs and performance issues related to the use of annotations backed by `MGLAnnotationImage`. The limits on the number and size of images and glyphs has been effectively eliminated and should now depend on hardware constraints. These fixes also apply to images used to represent icons in `MGLSymbolStyleLayer`. ([#9213](https://github.com/mapbox/mapbox-gl-native/pull/9213))
* Added an `overlays` property to `MGLMapView`. ([#8617](https://github.com/mapbox/mapbox-gl-native/pull/8617))
* Selecting an annotation no longer sets the user tracking mode to `MGLUserTrackingModeNone`. ([#10094](https://github.com/mapbox/mapbox-gl-native/pull/10094))
* Added `-[MGLMapView cameraThatFitsShape:direction:edgePadding:]` to get a camera with zoom level and center coordinate computed to fit a shape. ([#10107](https://github.com/mapbox/mapbox-gl-native/pull/10107))
* Added support selection of shape and polyline annotations.([#9984](https://github.com/mapbox/mapbox-gl-native/pull/9984))
* Fixed an issue where view annotations could be slightly misaligned. View annotation placement is now rounded to the nearest pixel. ([#10219](https://github.com/mapbox/mapbox-gl-native/pull/10219))
* Fixed an issue where a shape annotation callout was not displayed if the centroid was not visible. ([#10255](https://github.com/mapbox/mapbox-gl-native/pull/10255))

### User interaction

* Users of VoiceOver can now swipe left and right to navigate among visible places, points of interest, and roads. ([#9950](https://github.com/mapbox/mapbox-gl-native/pull/9950))
* Increased the default maximum zoom level from 20 to 22. ([#9835](https://github.com/mapbox/mapbox-gl-native/pull/9835))
* Fixed an issue where the same value was passed in as the `oldCamera` and `newCamera` parameters to the `-[MGLMapViewDelegate mapView:shouldChangeFromCamera:toCamera:]` method. ([#10433](https://github.com/mapbox/mapbox-gl-native/pull/10433))

### Other changes

* Added a Bulgarian localization. ([#10309](https://github.com/mapbox/mapbox-gl-native/pull/10309))
* Fixed an issue that could cause line label rendering glitches when the line geometry is projected to a point behind the plane of the camera. ([#9865](https://github.com/mapbox/mapbox-gl-native/pull/9865))
* Fixed an issue that could cause a crash when using `-[MGLMapView flyToCamera:completionHandler:]` and related methods with zoom levels at or near the maximum value. ([#9381](https://github.com/mapbox/mapbox-gl-native/pull/9381))
* Added `-[MGLMapView showAttribution:]` to allow custom attribution buttons to show the default attribution interface. ([#10085](https://github.com/mapbox/mapbox-gl-native/pull/10085))
* Fixed a conflict between multiple copies of SMCalloutView in a project. ([#10183](https://github.com/mapbox/mapbox-gl-native/pull/10183))
* Fixed a crash when enabling the scale bar in iOS 8. ([#10241](https://github.com/mapbox/mapbox-gl-native/pull/10241))

## 3.6.4 - September 25, 2017

* Fixed an issue where stale (but still valid) map data could be ignored in offline mode. ([#10012](https://github.com/mapbox/mapbox-gl-native/pull/10012))

## 3.6.3 - September 15, 2017

* Added the option to display an always-on heading indicator with the default user location annotation, controlled via the `MGLMapView.showsUserHeadingIndicator` property. ([#9886](https://github.com/mapbox/mapbox-gl-native/pull/9886))
* Fixed an issue where user heading tracking mode would update too frequently. ([#9845](https://github.com/mapbox/mapbox-gl-native/pull/9845))
* Added support for iOS 11 location usage descriptions. ([#9869](https://github.com/mapbox/mapbox-gl-native/pull/9869))
* Fixed an issue where `MGLUserLocation.location` did not follow its documented initialization behavior. This property will now properly return `nil` until the user’s location has been determined. ([#9639](https://github.com/mapbox/mapbox-gl-native/pull/9639))
* `MGLMapView`’s `minimumZoomLevel` and `maximumZoomLevel` properties are now available in Interface Builder’s Attributes inspector. ([#9729](https://github.com/mapbox/mapbox-gl-native/pull/9729))
* Deprecated `+[MGLStyle trafficDayStyleURL]` and `+[MGLStyle trafficNightStyleURL]` with no replacement method. To use the Traffic Day and Traffic Night styles going forward, we recommend that you use the underlying URL. ([#9918](https://github.com/mapbox/mapbox-gl-native/pull/9918))
* Fixed a crash that sometimes occurred when a map view's view controller was deallocated. ([#9995](https://github.com/mapbox/mapbox-gl-native/pull/9995))

## 3.6.2 - August 18, 2017

* Added an `MGLStyle.localizesLabels` property, off by default, that localizes any Mapbox Streets–sourced symbol layer into the user’s preferred language. ([#9582](https://github.com/mapbox/mapbox-gl-native/pull/9582))
* Added an additional camera method to MGLMapView that accepts an edge padding parameter. ([#9651](https://github.com/mapbox/mapbox-gl-native/pull/9651))
* Fixed an issue with the scaling of the user location annotation’s horizontal accuracy indicator. ([#9721](https://github.com/mapbox/mapbox-gl-native/pull/9721))
* Fixed an issue that caused `-[MGLShapeSource featuresMatchingPredicate:]` and `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` to always return an empty array. ([#9784](https://github.com/mapbox/mapbox-gl-native/pull/9784))

## 3.6.1 - July 28, 2017

* Reduced the size of the dynamic framework by optimizing symbol visibility. ([#7604](https://github.com/mapbox/mapbox-gl-native/pull/7604))
* Fixed an issue where the attribution button would have its custom tint color reset when the map view received a tint color change notification, such as when an alert controller was presented. ([#9598](https://github.com/mapbox/mapbox-gl-native/pull/9598))
* Improved the behavior of zoom gestures when the map reaches the minimum zoom limit. ([#9626](https://github.com/mapbox/mapbox-gl-native/pull/9626))
* Fixed an issue where tilt gesture was triggered with two fingers aligned vertically and panning down. ([#9571](https://github.com/mapbox/mapbox-gl-native/pull/9571))
* Bitcode symbol maps (.bcsymbolmap files) are now included with the dynamic framework. ([#9613](https://github.com/mapbox/mapbox-gl-native/pull/9613))

## 3.6.0 - June 29, 2017

### Packaging

* Xcode 8.0 or higher is now recommended for using this SDK. ([#8775](https://github.com/mapbox/mapbox-gl-native/pull/8775))
* Fixed an issue in the static framework where localizations would never load. ([#9074](https://github.com/mapbox/mapbox-gl-native/pull/9074))
* Updated MGLMapView’s logo view to display [the new Mapbox logo](https://www.mapbox.com/blog/new-mapbox-logo/). ([#8771](https://github.com/mapbox/mapbox-gl-native/pull/8771), [#8773](https://github.com/mapbox/mapbox-gl-native/pull/8773))
* Added a Hungarian localization. ([#9347](https://github.com/mapbox/mapbox-gl-native/pull/9347))

### Styles

* Added support for 3D extrusion of buildings and other polygonal features via the `MGLFillExtrusionStyleLayer` class and the `fill-extrusion` layer type in style JSON. ([#8431](https://github.com/mapbox/mapbox-gl-native/pull/8431))
* MGLMapView and MGLTilePyramidOfflineRegion now default to version 10 of the Mapbox Streets style. Similarly, several style URL class methods of MGLStyle return URLs to version 10 styles. Unversioned variations of these methods are no longer deprecated. `MGLStyleDefaultVersion` should no longer be used with any style other than Streets. ([#6301](https://github.com/mapbox/mapbox-gl-native/pull/6301))
* Added class methods to MGLStyle that correspond to the new [Traffic Day and Traffic Night](https://www.mapbox.com/blog/live-traffic-maps/) styles. ([#6301](https://github.com/mapbox/mapbox-gl-native/pull/6301))
* MGLSymbolStyleLayer’s `iconImageName`, `iconScale`, `textFontSize`, `textOffset`, and `textRotation` properties can now be set to a source or composite function. ([#8544](https://github.com/mapbox/mapbox-gl-native/pull/8544), [#8590](https://github.com/mapbox/mapbox-gl-native/pull/8590), [#8592](https://github.com/mapbox/mapbox-gl-native/pull/8592), [#8593](https://github.com/mapbox/mapbox-gl-native/pull/8593))
* Fixed an issue where setting the `MGLVectorStyleLayer.predicate` property failed to take effect if the relevant source was not in use by a visible layer at the time. ([#8653](https://github.com/mapbox/mapbox-gl-native/pull/8653))
* Fixed an issue preventing programmatically added style layers from appearing in already cached tiles. ([#8954](https://github.com/mapbox/mapbox-gl-native/pull/8954))
* Fixed an issue causing a composite function’s highest zoom level stop to be misinterpreted. ([#8613](https://github.com/mapbox/mapbox-gl-native/pull/8613), [#8790](https://github.com/mapbox/mapbox-gl-native/pull/8790))
* Fixed an issue where re-adding a layer that had been previously removed from a style would reset its paint properties. Moved initializers for `MGLTileSource`, `MGLStyleLayer`, and `MGLForegroundStyleLayer` to their concrete subclasses; because these classes were already intended for initialization only via concrete subclasses, this should have no developer impact. ([#8626](https://github.com/mapbox/mapbox-gl-native/pull/8626))
* Fixed a crash that occurred when removing a source that was still being used by one or more style layers. Since this is a programming error, a warning is logged to the console instead. ([#9129](https://github.com/mapbox/mapbox-gl-native/pull/9129))
* Feature querying results now account for any changes to a feature’s size caused by a source or composite style function. ([#8665](https://github.com/mapbox/mapbox-gl-native/pull/8665))
* Fixed the behavior of composite functions that specify fractional zoom level stops. ([#9289](https://github.com/mapbox/mapbox-gl-native/pull/9289))
* Letter spacing is now disabled in Arabic text so that ligatures are drawn correctly. ([#9062](https://github.com/mapbox/mapbox-gl-native/pull/9062))
* Improved the performance of styles using source and composite style functions. ([#9185](https://github.com/mapbox/mapbox-gl-native/pull/9185), [#9257](https://github.com/mapbox/mapbox-gl-native/pull/9257))

### Annotations

* Added a new initializer to `MGLAnnotationView` so that it is possible to create a new instance with an associated annotation object. ([#9029](https://github.com/mapbox/mapbox-gl-native/pull/9029))
* Added a new `rotatesToMatchCamera` property to `MGLAnnotationView` that, when set to true, causes the annotation view to rotate along with the map's rotation angle giving the appearance that the annoation view is pinned to the map. ([#9147](https://github.com/mapbox/mapbox-gl-native/pull/9147))
* Fixed an issue causing a view-backed annotation to disappear immediately instead of animating when the annotation’s `coordinate` property is set to a value outside the current viewport. ([#8565](https://github.com/mapbox/mapbox-gl-native/pull/8565))
* Fixed an issue in which `MGLMapView` overrode the tint colors of its annotation views. ([#8789](https://github.com/mapbox/mapbox-gl-native/pull/8789))
* Fixed an issue causing annotation views to persist in the map’s annotation container view even after their associated annotations were removed. ([#9025](https://github.com/mapbox/mapbox-gl-native/pull/9025))
* The `MGLPolyline.coordinate` and `MGLPolygon.coordinate` properties now return the midpoint and centroid, respectively, instead of the first coordinate. ([#8713](https://github.com/mapbox/mapbox-gl-native/pull/8713))

### User interaction

* Added a scale bar to `MGLMapView` that indicates the scale of the map. ([#7631](https://github.com/mapbox/mapbox-gl-native/pull/7631))
* Fixed an issue causing the map to go blank during a flight animation that travels a very short distance. ([#9199](https://github.com/mapbox/mapbox-gl-native/pull/9199))
* Fixed an issue where gesture recognizers associated with map view interactivity were not disabled when their related interactions were disabled. ([#8304](https://github.com/mapbox/mapbox-gl-native/pull/8304))
* Fixed an issue preventing the Mapbox Telemetry confirmation dialog from appearing when opened from within a map view in a modal view controller. ([#9027](https://github.com/mapbox/mapbox-gl-native/pull/9027))
* Corrected the size of MGLMapView’s compass. ([#9060](https://github.com/mapbox/mapbox-gl-native/pull/9060))
* The Improve This Map button in the attribution action sheet now leads to a feedback tool that matches MGLMapView’s rotation and pitch. `-[MGLAttributionInfo feedbackURLAtCenterCoordinate:zoomLevel:]` no longer respects the feedback URL specified in TileJSON. ([#9078](https://github.com/mapbox/mapbox-gl-native/pull/9078))
* `-[MGLMapViewDelegate mapView:shouldChangeFromCamera:toCamera:]` can now block any panning caused by a pinch gesture. ([#9344](https://github.com/mapbox/mapbox-gl-native/pull/9344))
* If the user taps on the map while it is flying to the user’s location, the user dot no longer appears in the incorrect location. ([#7916](https://github.com/mapbox/mapbox-gl-native/pull/7916))
* Improved the responsiveness of the tilt gesture by reducing the initial recognition delay. ([#9386](https://github.com/mapbox/mapbox-gl-native/pull/9386))

### Other changes

* Fixed a crash that occurred when accessing the `MGLMultiPolygon.coordinate` property. ([#8713](https://github.com/mapbox/mapbox-gl-native/pull/8713))
* Fixed a crash or console spew when MGLMapView is initialized with a frame smaller than 64 points wide by 64 points tall. ([#8562](https://github.com/mapbox/mapbox-gl-native/pull/8562))
* Fixed an issue that caused the compass and scale bar to underlap navigation and tab bars. ([#7716](https://github.com/mapbox/mapbox-gl-native/pull/7716))
* The error passed into `-[MGLMapViewDelegate mapViewDidFailLoadingMap:withError:]` now includes a more specific description and failure reason. ([#8418](https://github.com/mapbox/mapbox-gl-native/pull/8418))
* Improved CPU and battery performance while animating a tilted map’s camera in an area with many labels. ([#9031](https://github.com/mapbox/mapbox-gl-native/pull/9031))
* Fixed an issue rendering polylines that contain duplicate vertices. ([#8808](https://github.com/mapbox/mapbox-gl-native/pull/8808))
* Added struct boxing to `MGLCoordinateSpan`, `MGLCoordinateBounds`, `MGLOfflinePackProgress`, and `MGLTransition`. ([#9343](https://github.com/mapbox/mapbox-gl-native/pull/9343))

## 3.5.4 - May 9, 2017

* Fixed an issue that caused view backed annotations to become detached from the map view during pan gestures combined with animations of annotation view size or when the annotation view had no size but contained subviews with a non-zero size. ([#8926](https://github.com/mapbox/mapbox-gl-native/pull/8926))

## 3.5.3 - May 2, 2017

* Fixed an issue that prevented the attribution `UIAlertController` from showing in modal hierarchies. ([#8837](https://github.com/mapbox/mapbox-gl-native/pull/8837))

## 3.5.2 - April 7, 2017

* Fixed an issue that caused a crash when the user location annotation was presenting a callout view and the map was moved. ([#8686](https://github.com/mapbox/mapbox-gl-native/pull/8686))
* This release was built with Xcode 8.3.1, which fixed [a significant bitcode issue](http://www.openradar.me/31302382) introduced in Xcode 8.3 that caused Mapbox iOS SDK 3.5.1 to be 2× larger than 3.5.0.

## 3.5.1 - April 5, 2017

* Fixed an issue that caused the return type of a map view delegate method to bridge incorrectly to applications written in Swift. ([#8541](https://github.com/mapbox/mapbox-gl-native/pull/8541))
* Fixed a crash that could occur when calling `-[MGLShapeSource featuresMatchingPredicate:]` or `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]`. ([#8553](https://github.com/mapbox/mapbox-gl-native/pull/8553))
* Fixed a crash that could occur after adding view-backed annotations to the map. ([#8513](https://github.com/mapbox/mapbox-gl-native/pull/8513))
* Renamed the “Data-Driven Styling” guide to “Using Style Functions at Runtime” and clarified the meaning of data-driven styling in the guide’s discussion of runtime style functions. ([#8627](https://github.com/mapbox/mapbox-gl-native/pull/8627))

## 3.5.0 - March 21, 2017

### Packaging

* The minimum deployment target for this SDK is now iOS 8. ([#8129](https://github.com/mapbox/mapbox-gl-native/pull/8129))
* Added support for the Carthage dependency manager. See [our website](https://www.mapbox.com/ios-sdk/) for setup instructions. ([#8257](https://github.com/mapbox/mapbox-gl-native/pull/8257))
* While running your application in the iOS Simulator, you will receive a notice in the console if a newer version of this SDK is available. ([#8282](https://github.com/mapbox/mapbox-gl-native/pull/8282))

### Internationalization

* Added support for right-to-left text and Arabic ligatures in labels. ([#6984](https://github.com/mapbox/mapbox-gl-native/pull/6984), [#7123](https://github.com/mapbox/mapbox-gl-native/pull/7123))
* Improved the line wrapping behavior of point-placed labels, especially labels written in Chinese and Japanese. ([#6828](https://github.com/mapbox/mapbox-gl-native/pull/6828), [#7446](https://github.com/mapbox/mapbox-gl-native/pull/7446))
* CJK characters now remain upright in vertically oriented labels that have line placement, such as road labels. ([#7114](https://github.com/mapbox/mapbox-gl-native/issues/7114))
* Added Catalan, Chinese (Simplified and Traditional), Dutch, Finnish, French, German, Japanese, Lithuanian, Polish, Portuguese (Brazilian), Russian, Spanish, Swedish, Ukrainian, and Vietnamese localizations. ([#7316](https://github.com/mapbox/mapbox-gl-native/pull/7316), [#7899](https://github.com/mapbox/mapbox-gl-native/pull/7899), [#7999](https://github.com/mapbox/mapbox-gl-native/pull/7999), [#8113](https://github.com/mapbox/mapbox-gl-native/pull/8113), [#8256](https://github.com/mapbox/mapbox-gl-native/pull/8256))

### Styles

* Added support for data-driven styling in the form of source and composite style functions. `MGLStyleFunction` is now an abstract class, with `MGLCameraStyleFunction` providing the behavior of `MGLStyleFunction` in previous releases. New `MGLStyleFunction` subclasses allow you to vary a style attribute by the values of attributes of features in the source. ([#7596](https://github.com/mapbox/mapbox-gl-native/pull/7596))
* Added `circleStrokeColor`, `circleStrokeWidth`, and `circleStrokeOpacity` properties to MGLCircleStyleLayer and support for corresponding properties in style JSON files. ([#7356](https://github.com/mapbox/mapbox-gl-native/pull/7356))
* Point-placed labels in symbol style layers are now placed at more optimal locations within polygons. ([#7465](https://github.com/mapbox/mapbox-gl-native/pull/7465))
* Fixed flickering that occurred when manipulating a style layer. ([#7616](https://github.com/mapbox/mapbox-gl-native/pull/7616))
* Symbol style layers can now render point collections (known as multipoints in GeoJSON). ([#7445](https://github.com/mapbox/mapbox-gl-native/pull/7445))
* Added a `transition` property to MGLStyle to customize the timing of changes to style layers. ([#7711](https://github.com/mapbox/mapbox-gl-native/pull/7711))
* Added properties to MGLStyleLayer subclasses to customize the timing of transitions between values of individual attributes. ([#8225](https://github.com/mapbox/mapbox-gl-native/pull/8225))
* Fixed an issue causing lines and text labels toward the top of the map view to appear blurry when the map is tilted. ([#7444](https://github.com/mapbox/mapbox-gl-native/pull/7444))
* Fixed incorrect interpolation of style functions in Boolean-typed style attributes. ([#7526](https://github.com/mapbox/mapbox-gl-native/pull/7526))
* Removed support for the `ref` property in layers in style JSON files. ([#7586](https://github.com/mapbox/mapbox-gl-native/pull/7586))
* Fixed an issue that collapsed consecutive newlines within text labels. ([#7446](https://github.com/mapbox/mapbox-gl-native/pull/7446))
* Fixed artifacts when drawing particularly acute line joins. ([#7786](https://github.com/mapbox/mapbox-gl-native/pull/7786))
* Fixed an issue in which a vector style layer predicate involving the `$id` key path would exclude all features from the layer. ([#7989](https://github.com/mapbox/mapbox-gl-native/pull/7989), [#7971](https://github.com/mapbox/mapbox-gl-native/pull/7971))
* Fixed an issue causing vector style layer predicates to be evaluated as if each feature had a `$type` attribute of 1, 2, or 3. The `$type` key path can now be compared to `Point`, `LineString`, or `Polygon`, as described in the documentation. ([#7971](https://github.com/mapbox/mapbox-gl-native/pull/7971))
* When setting an `MGLShapeSource`’s shape to an `MGLFeature` instance, any `UIColor` attribute value is now converted to the equivalent CSS string representation for use with `MGLInterpolationModeIdentity` in style functions. ([#8025](https://github.com/mapbox/mapbox-gl-native/pull/8025))
* An exception is no longer thrown if layers or sources are removed from a style before they are added. ([#7962](https://github.com/mapbox/mapbox-gl-native/pull/7962))
* Renamed MGLStyleConstantValue to MGLConstantStyleValue. For compatibility with previous releases, MGLStyleConstantValue is now an alias of MGLConstantStyleValue. ([#8090](https://github.com/mapbox/mapbox-gl-native/pull/8090))
* Fixed a crash that could occur when switching styles after adding an MGLSource to the style. ([#8298](https://github.com/mapbox/mapbox-gl-native/pull/8298))

### Annotations and user interaction

* Added a method to MGLMapViewDelegate, `-mapView:shouldChangeFromCamera:toCamera:`, that you can implement to restrict which parts the user can navigate to using gestures. ([#5584](https://github.com/mapbox/mapbox-gl-native/pull/5584))
* Annotations are no longer deselected when the map is panned or zoomed, even if the annotation moves out of the visible bounds. ([#8022](https://github.com/mapbox/mapbox-gl-native/pull/8022))
* Changing the coordinates of a point annotation no longer deselects the annotation. ([#8269](https://github.com/mapbox/mapbox-gl-native/pull/8269))
* Fixed an issue that could cause a crash when point annotations were added and removed while simultaneously querying source features. ([#8374](https://github.com/mapbox/mapbox-gl-native/pull/8374))
* Fixed an issue preventing MGLMapView from adding a polyline annotation with the same coordinates as a polygon annotation. ([#8355](https://github.com/mapbox/mapbox-gl-native/pull/8355))
* Fixed an issue where translucent, non-view-backed point annotations along tile boundaries would be drawn darker than expected. ([#6832](https://github.com/mapbox/mapbox-gl-native/pull/6832))
* Double-tap and two-finger tap gestures now zoom to the nearest integer zoom level. ([#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027))
* The `MGLAnnotationView.annotation` property is now read-write. ([#8139](https://github.com/mapbox/mapbox-gl-native/pull/8139))
* Enabled the one-finger zoom gesture on iPad. To execute this gesture, tap twice; on second tap, hold your finger on the map and pan up to zoom in, or down to zoom out. ([#8379](https://github.com/mapbox/mapbox-gl-native/pull/8379))

### Networking and offline maps

* Offline pack notifications are now posted by `MGLOfflinePack` instances instead of the shared `MGLOfflineStorage` object. For backwards compatibility, the `userInfo` dictionary still indicates the pack’s state and progress. ([#7952](https://github.com/mapbox/mapbox-gl-native/pull/7952))
* Fixed a memory leak in MGLMapView. ([#7956](https://github.com/mapbox/mapbox-gl-native/pull/7956))
* Fixed an issue that could prevent a cached style from appearing while the device is offline. ([#7770](https://github.com/mapbox/mapbox-gl-native/pull/7770))
* Fixed an issue that could prevent a style from loading when reestablishing a network connection. ([#7902](https://github.com/mapbox/mapbox-gl-native/pull/7902))
* `MGLOfflineStorage` instances now support a delegate conforming to `MGLOfflineStorageDelegate`, which allows altering URLs before they are requested from the Internet. ([#8084](https://github.com/mapbox/mapbox-gl-native/pull/8084))

### Other changes

* Fixed an issue that, among other things, caused various islands to disappear at certain zoom levels. ([#7621](https://github.com/mapbox/mapbox-gl-native/pull/7621))
* Added a method to MGLMapView that allows you to specify a predicate when querying for visible features. ([#8256](https://github.com/mapbox/mapbox-gl-native/pull/8246))
* Fixed flickering that occurred when panning past the antimeridian. ([#7574](https://github.com/mapbox/mapbox-gl-native/pull/7574))
* Fixed an issue that sometimes caused crashes when the SDK interacted with the file system in the background. ([#8125](https://github.com/mapbox/mapbox-gl-native/pull/8125))
* Added a `MGLDistanceFormatter` class for formatting geographic distances. ([#7888](https://github.com/mapbox/mapbox-gl-native/pull/7888))
* Fixed an issue that was causing the system location indicator to stay on in background after telemetry was disabled. ([#7833](https://github.com/mapbox/mapbox-gl-native/pull/7833))
* Added support for predicates in rendered feature querying [8256](https://github.com/mapbox/mapbox-gl-native/pull/8246)
* Added a nightly build of the dynamic framework. ([#8337](https://github.com/mapbox/mapbox-gl-native/pull/8337))

## 3.4.2 - February 21, 2017

This is the final scheduled version of the Mapbox iOS SDK that supports iOS 7. ([#8129](https://github.com/mapbox/mapbox-gl-native/pull/8129))

* A programmatic change to an MGLMapView’s camera no longer resets the user tracking mode. ([#7856](https://github.com/mapbox/mapbox-gl-native/pull/7856))
* Improved the performance of trivial camera animations. ([#7125](https://github.com/mapbox/mapbox-gl-native/pull/7125))
* Added a guide detailing the built-in gesture recognizers and various ways to configure them. ([#7937](https://github.com/mapbox/mapbox-gl-native/pull/7937))

## 3.4.1 - January 25, 2017

* Fixed a build error in the static framework flavor of this SDK caused by a missing header. ([#7844](https://github.com/mapbox/mapbox-gl-native/pull/7844))
* Fixed an issue causing MGLMapView’s `camera`’s `heading` to be set to a negative value, indicating an undefined heading, when the map view faces northwest. The heading is now wrapped to between zero and 360 degrees, for consistency with MGLMapView’s `direction` property. ([#7724](https://github.com/mapbox/mapbox-gl-native/pull/7724))
* Fixed an issue where MGLMapView could initially flash black before loading. ([#7859](https://github.com/mapbox/mapbox-gl-native/pull/7859))
* Deprecated the style class methods in MGLStyle. ([#7785](https://github.com/mapbox/mapbox-gl-native/pull/7785))

## 3.4.0 - January 20, 2017

### Packaging

* Xcode 7.3 or above is required for using this SDK. ([#6059](https://github.com/mapbox/mapbox-gl-native/issues/6059))
* Clarified that the `-ObjC` linker flag is required for linking against the static framework distribution of this SDK. ([#6213](https://github.com/mapbox/mapbox-gl-native/pull/6213))
* The API reference has a sharper look. ([#7422](https://github.com/mapbox/mapbox-gl-native/pull/7422))
* Added documentation for the Info.plist keys used by this SDK. ([#6833](https://github.com/mapbox/mapbox-gl-native/pull/6833))

### Styles and data

* A new runtime styling API allows you to adjust the style and content of the base map dynamically. All the options available in [Mapbox Studio](https://www.mapbox.com/studio/) are now exposed via MGLStyle and subclasses of MGLStyleLayer and MGLSource. ([#5727](https://github.com/mapbox/mapbox-gl-native/pull/5727))
* MGLMapView’s `styleURL` property can now be set to an absolute file URL. ([#6026](https://github.com/mapbox/mapbox-gl-native/pull/6026))
* When creating an MGLShapeSource, you can now specify options for clustering point features within the shape source. Similarly, GeoJSON sources specified by the stylesheet at design time can specify the `cluster`, `clusterMaxZoom`, and `clusterRadius` attributes. ([#5724](https://github.com/mapbox/mapbox-gl-native/pull/5724))
* Added [quadkey](https://msdn.microsoft.com/en-us/library/bb259689.aspx) support and limited WMS support in raster tile URL templates. ([#5628](https://github.com/mapbox/mapbox-gl-native/pull/5628))
* When creating an MGLTileSource, you can now specify that the tile URLs use [TMS](https://en.wikipedia.org/wiki/Tile_Map_Service) coordinates by setting `MGLTileSourceOptionTileCoordinateSystem` to `MGLTileCoordinateSystemTMS`. TileJSON files can specify `"scheme": "tms"`. ([#2270](https://github.com/mapbox/mapbox-gl-native/pull/2270))
* Fixed an issue causing abstract MGLMultiPointFeature objects to be returned in feature query results. Now concrete MGLPointCollectionFeature objects are returned. MGLMultiPointFeature is now an alias of MGLPointCollectionFeature. ([#6742](https://github.com/mapbox/mapbox-gl-native/pull/6742))
* Fixed rendering artifacts and missing glyphs that occurred after viewing a large number of CJK characters on the map. ([#5908](https://github.com/mapbox/mapbox-gl-native/pull/5908))
* `-[MGLMapView resetPosition]` now resets to the current style’s default center coordinates, zoom level, direction, and pitch, if specified. ([#6127](https://github.com/mapbox/mapbox-gl-native/pull/6127))
* Fixed an issue where feature querying sometimes failed to return the expected features when the map was tilted. ([#6773](https://github.com/mapbox/mapbox-gl-native/pull/6773))
* MGLFeature’s `attributes` and `identifier` properties are now writable. ([#6728](https://github.com/mapbox/mapbox-gl-native/pull/6728))
* The action sheet that appears when tapping the information button in the bottom-right corner now lists the correct attribution for the current style. ([#5999](https://github.com/mapbox/mapbox-gl-native/pull/5999))
* Added support for MGLSymbolStyleLayer’s `textPitchAlignment` property and the corresponding style JSON property for improved street label legibility on a tilted map. ([#5288](https://github.com/mapbox/mapbox-gl-native/pull/5288))
* Added support for MGLSymbolStyleLayer’s `iconTextFit` and `iconTextFitPadding` properties and the corresponding style JSON properties, allowing the background of a shield to automatically resize to fit the shield’s text. ([#5334](https://github.com/mapbox/mapbox-gl-native/pull/5334))
* Added support for MGLSymbolStyleLayer’s `circlePitchScale` property and the corresponding style JSON property, allowing circle features in a tilted base map to scale or remain the same size as the viewing distance changes. ([#5576](https://github.com/mapbox/mapbox-gl-native/pull/5576))
* The `identifier` property of an MGLFeature may now be either a number or string. ([#5514](https://github.com/mapbox/mapbox-gl-native/pull/5514))
* If MGLMapView is unable to obtain or parse a style, it now calls its delegate’s `-mapViewDidFailLoadingMap:withError:` method. ([#6145](https://github.com/mapbox/mapbox-gl-native/pull/6145))
* Added the `-[MGLMapViewDelegate mapView:didFinishLoadingStyle:]` delegate method, which offers the earliest opportunity to modify the layout or appearance of the current style before the map view is displayed to the user. ([#6636](https://github.com/mapbox/mapbox-gl-native/pull/6636))
* Fixed crashes that could occur when loading a malformed stylesheet. ([#5736](https://github.com/mapbox/mapbox-gl-native/pull/5736))
* Fixed an issue causing stepwise zoom functions to be misinterpreted. ([#6328](https://github.com/mapbox/mapbox-gl-native/pull/6328))
* A source’s tiles are no longer rendered when the map is outside the source’s supported zoom levels. ([#6345](https://github.com/mapbox/mapbox-gl-native/pull/6345))
* Improved style parsing performance. ([#6170](https://github.com/mapbox/mapbox-gl-native/pull/6170))
* Improved feature querying performance. ([#6514](https://github.com/mapbox/mapbox-gl-native/pull/6514))
* Fixed an issue where shapes that cannot currently be visually represented as annotations were still shown on the map as point annotations. ([#6764](https://github.com/mapbox/mapbox-gl-native/issues/6764))

### User location

* The user dot now animates between user locations when user tracking is disabled. ([#6215](https://github.com/mapbox/mapbox-gl-native/pull/6215))
* To customize the appearance of the user location annotation, subclass the newly added MGLUserLocationAnnotationView class and implement `-[MGLMapViewDelegate mapView:viewForAnnotation:]`. ([#5882](https://github.com/mapbox/mapbox-gl-native/pull/5882))
* `-[MGLMapView viewForAnnotation:]` now returns the user location annotation view when given the user location annotation. ([#6957](https://github.com/mapbox/mapbox-gl-native/pull/6957))
* Fixed an issue causing the user dot’s accuracy ring to wobble while zooming in and out. ([#6019](https://github.com/mapbox/mapbox-gl-native/pull/6019))
* Heading accuracy indicator sizing has been changed to appear more precise. ([#6120](https://github.com/mapbox/mapbox-gl-native/pull/6120))
* Fixed an issue that caused the map to not update to reflect the centerOffset when the user location was tracked. ([#6216](https://github.com/mapbox/mapbox-gl-native/pull/6216))

### Annotations

* Added new methods to MGLMultiPoint for changing the vertices along a polyline annotation or the exterior of a polygon annotation. ([#6565](https://github.com/mapbox/mapbox-gl-native/pull/6565))
* Added new APIs to MGLMapView to query for visible annotations. Combined with `-[MGLMapView viewForAnnotation:]`, these APIs can be used to access all visible annotation views. ([#6061](https://github.com/mapbox/mapbox-gl-native/pull/6061))
* Shape, feature, and annotation classes now conform to NSSecureCoding. ([#6559](https://github.com/mapbox/mapbox-gl-native/pull/6559))
* Fixed an issue causing offscreen annotation views to be updated even when they were in the reuse queue. ([#5987](https://github.com/mapbox/mapbox-gl-native/pull/5987))
* Fixed an issue preventing MGLAnnotationView from animating when its coordinate changes. ([#6215](https://github.com/mapbox/mapbox-gl-native/pull/6215))
* Fixed an issue causing the wrong annotation view to be selected when tapping an annotation view with a center offset applied. ([#5931](https://github.com/mapbox/mapbox-gl-native/pull/5931))
* Fixed an issue that assigned annotation views to polyline and polygon annotations. ([#5770](https://github.com/mapbox/mapbox-gl-native/pull/5770))
* Fixed an issue causing the callout view to be dismissed when panning around. ([#6676](https://github.com/mapbox/mapbox-gl-native/pull/6676))
* Per documentation, the first and last coordinates in an MGLPolygon must be identical in order for the polygon to draw correctly. The same is true for an MGLPolygon’s interior polygon. ([#5514](https://github.com/mapbox/mapbox-gl-native/pull/5514))
* To make an MGLPolyline or MGLPolygon span the antimeridian, specify coordinates with longitudes greater than 180° or less than −180°. ([#6088](https://github.com/mapbox/mapbox-gl-native/pull/6088))
* Various method arguments that are represented as C arrays of `CLLocationCoordinate2D` instances have been marked `const` to streamline bridging to Swift. ([#7215](https://github.com/mapbox/mapbox-gl-native/pull/7215))
* Fixed an issue that caused an annotation view to disappear if it isn’t created using the annotation view reuse queue. ([#6485](https://github.com/mapbox/mapbox-gl-native/pull/6485))
* Fixed an issue that could reset user-added transformations on annotation views. ([#6166](https://github.com/mapbox/mapbox-gl-native/pull/6166))
* Improved the performance of relocating a non-view-backed point annotation by changing its `coordinate` property. ([#5385](https://github.com/mapbox/mapbox-gl-native/pull/5385))
* Fixed an issue that caused an assertion failure if a `MGLShapeCollection` (a GeoJSON GeometryCollection) was created with an empty array of shapes. ([#7632](https://github.com/mapbox/mapbox-gl-native/pull/7632))
* Improved the precision of annotations at zoom levels greater than 18. ([#5517](https://github.com/mapbox/mapbox-gl-native/pull/5517))

### Networking and offline maps

* Fixed an issue preventing an MGLMapView from loading tiles while an offline pack is downloading. ([#6446](https://github.com/mapbox/mapbox-gl-native/pull/6446))
* Fixed a crash that could occur when the device is disconnected while downloading an offline pack. ([#6293](https://github.com/mapbox/mapbox-gl-native/pull/6293))
* Fixed a crash that occurred when encountering a rate-limit error in response to a network request. ([#6223](https://github.com/mapbox/mapbox-gl-native/pull/6223))
* Fixed an issue causing an MGLOfflinePack’s progress to continue to update after calling `-suspend`. ([#6186](https://github.com/mapbox/mapbox-gl-native/pull/6186))
* Fixed an issue preventing cached annotation images from displaying while the device is offline. ([#6358](https://github.com/mapbox/mapbox-gl-native/pull/6358))
* Added support for an `MGLMapboxAPIBaseURL` key in an app's `Info.plist` in order to customize the base URL used for retrieving map data, styles, and other resources. ([#6709](https://github.com/mapbox/mapbox-gl-native/pull/6709))
* Query parameters are no longer stripped from mapbox: URLs used as resource URLs. ([#6182](https://github.com/mapbox/mapbox-gl-native/pull/6182), [#6432](https://github.com/mapbox/mapbox-gl-native/pull/6432))
* Database errors are now logged to the console. ([#6291](https://github.com/mapbox/mapbox-gl-native/pull/6291))

### Other changes

* Raster tiles such as those from Mapbox Satellite are now cached, eliminating flashing while panning back and forth. ([#7091](https://github.com/mapbox/mapbox-gl-native/pull/7091))
* Improved the performance of symbol style layers. ([#7025](https://github.com/mapbox/mapbox-gl-native/pull/7025))
* As the user zooms in, tiles from lower zoom levels are scaled up until tiles for higher zoom levels are loaded. ([#5143](https://github.com/mapbox/mapbox-gl-native/pull/5143))
* Notification names and user info keys are now string enumeration values for ease of use in Swift. ([#6794](https://github.com/mapbox/mapbox-gl-native/pull/6794))
* MGLMapDebugOverdrawVisualizationMask no longer has any effect in Release builds of the SDK. This debug mask has been disabled for performance reasons. ([#5555](https://github.com/mapbox/mapbox-gl-native/pull/5555))
* Fixed a typo in the documentation for the MGLCompassDirectionFormatter class. ([#5879](https://github.com/mapbox/mapbox-gl-native/pull/5879))
* The UITapGestureRecognizer on MGLMapView that is used for selecting annotations now fails if a tap does not select an annotation. ([#7246](https://github.com/mapbox/mapbox-gl-native/pull/7246))
* Fixed issues related to the visibility of sources in viewports less than 512 pixels wide or tall. ([#7438](https://github.com/mapbox/mapbox-gl-native/pull/7438))

## 3.3.7 - November 17, 2016

* This version is the same as 3.3.6 but it is built with Xcode 8 that produces a smaller binary. Developers using this version in Swift applications built with Xcode 7.3.1 should use the  unstripped, `*-symbols` framework.

## 3.3.6 - November 9, 2016

* Fixed a crash that occurred during low-memory situations when multiple instances of MGLMapView were in the view hierarchy. The speculative fix in v3.3.5 has been reverted. ([#6972](https://github.com/mapbox/mapbox-gl-native/pull/6972))

## 3.3.5 - November 2, 2016

* Speculatively fixed an OpenGL rendering crash. ([#6844](https://github.com/mapbox/mapbox-gl-native/pull/6844))
* Fixed an issue with symbols not being properly stripped from the dynamic framework. The dSYM file included with the standard dynamic framework in previous releases (e.g., `mapbox-ios-sdk-3.3.4-dynamic.zip` or the `Mapbox-iOS-SDK` pod) could not be used to symbolicate crashes. ([#6531](https://github.com/mapbox/mapbox-gl-native/pull/6531))
* Simulator architecture slices are included in the included dSYM file, allowing you to symbolicate crashes that occur in the Simulator. ([#5740](https://github.com/mapbox/mapbox-gl-native/pull/5740))
* Fixed a crash that sometimes occurred when initializing an MGLMapView. ([#5932](https://github.com/mapbox/mapbox-gl-native/pull/5932))

## 3.3.4 - August 8, 2016

* Fixed an issue that caused the user dot to be selected when tapping an annotation that lies within the user dot’s accuracy circle. First attempt was [#5816](https://github.com/mapbox/mapbox-gl-native/pull/5816) in v3.3.2, which excluded the pulsing halo but not the accuracy circle. ([#5894](https://github.com/mapbox/mapbox-gl-native/pull/5894))

## 3.3.3 - July 29, 2016

* Fixed an issue where the style zoom levels were not respected when deciding when to render a layer. ([#5811](https://github.com/mapbox/mapbox-gl-native/issues/5811))

## 3.3.2 - July 28, 2016

* Speculatively fixed a crash that occurred when initializing an MGLMapView on iOS 7.x. ([#5791](https://github.com/mapbox/mapbox-gl-native/pull/5791))
* View-backed annotations no longer prevent the user from starting to pan the map. ([#5813](https://github.com/mapbox/mapbox-gl-native/pull/5813))
* Fixed an issue that caused the user dot to be selected when tapping an annotation that lies within the user dot’s accuracy circle. ([#5816](https://github.com/mapbox/mapbox-gl-native/pull/5816))

## 3.3.1 - July 19, 2016

* Fixed a crash that occurred when a sprite URL lacks a file extension. See [this comment](https://github.com/mapbox/mapbox-gl-native/issues/5722#issuecomment-233701251) to determine who may be affected by this bug. ([#5723](https://github.com/mapbox/mapbox-gl-native/pull/5723))
* Fixed an issue causing overlapping polylines and polygons to be drawn in undefined z-order. Shapes are always drawn in the order they are added to the map, from the oldest on the bottom to the newest on the top. ([#5710](https://github.com/mapbox/mapbox-gl-native/pull/5710))
* Fixed an issue preventing MGLMapView from changing its viewport when a single annotation was passed into `-[MGLMapView showAnnotations:animated:]`. ([#5693](https://github.com/mapbox/mapbox-gl-native/pull/5693))
* Fixed an issue causing polyline and polygon annotations to disappear when the zoom level is one less than the maximum zoom level. ([#5418](https://github.com/mapbox/mapbox-gl-native/pull/5418))
* Fixed a crash that occurred when a style or other resource URL has a query string. ([#5554](https://github.com/mapbox/mapbox-gl-native/pull/5554))
* If you subclass MGLAnnotationView, your implementation of `-setSelected:animated:` is now called with the correct value in the `animated` parameter, making it possible to animate a deselection. ([#5677](https://github.com/mapbox/mapbox-gl-native/pull/5677))
* The compass, Mapbox logo, and attribution button now accommodate the containing map view’s content insets. If your interface elements partially overlap the map view but do not affect the top and bottom layout guides, set the `automaticallyAdjustsScrollViewInsets` property to `NO` and set the `contentInset` property to a suitable value. ([#5671](https://github.com/mapbox/mapbox-gl-native/pull/5671))
* Added a property to MGLOfflineStorage, `countOfBytesCompleted`, that indicates the disk space occupied by all cached and offline resources. ([#5585](https://github.com/mapbox/mapbox-gl-native/pull/5585))

## 3.3.0 - July 14, 2016

### Styles and data

- Added methods to MGLMapView for obtaining the underlying map data rendered by the current style, along with additional classes to represent complex geometry in that data. ([#5110](https://github.com/mapbox/mapbox-gl-native/pull/5110))
- Improved performance viewing regions with large landcover polygons when viewing a style that uses the Mapbox Streets source. ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))
- Fixed a memory leak when using raster resources. ([#5141](https://github.com/mapbox/mapbox-gl-native/pull/5141))
- Rendering now occurs on the main thread, fixing a hang when calling `-[MGLMapView styleURL]` before the map view has fully loaded or while the application is in the background. ([#2909](https://github.com/mapbox/mapbox-gl-native/pull/2909))
- Added a `-reloadStyle:` action to MGLMapView to force a reload of the current style. ([#4728](https://github.com/mapbox/mapbox-gl-native/pull/4728))
- A more specific user agent string is now sent with style and tile requests. ([#4012](https://github.com/mapbox/mapbox-gl-native/pull/4012))
- Added a new option to `MGLMapDebugMaskOptions`, `MGLMapDebugOverdrawVisualizationMask`, that highlights overlapping drawing operations instead of the usual rendered output. ([#5403](https://github.com/mapbox/mapbox-gl-native/pull/5403))

### Interactivity

- The compass, user dot, and visible annotations are now accessible to VoiceOver users. ([#1496](https://github.com/mapbox/mapbox-gl-native/pull/1496))
- Added a method to MGLMapView, `-anchorPointForGesture:`, that you can override to anchor gestures at a point other than the user location. ([#5302](https://github.com/mapbox/mapbox-gl-native/pull/5302))
- Added a property to MGLMapView, `decelerationRate`, that allows you to speed up or slow down the drift animation at the end of a user gesture. You can also use this property to disable the drift animation entirely. ([#5504](https://github.com/mapbox/mapbox-gl-native/pull/5504))
- Improved responsiveness when zooming in then immediately panning around. ([#4595](https://github.com/mapbox/mapbox-gl-native/pull/4595))
- Added a new method, `-[MGLMapView cameraThatFitsCoordinateBounds:]`, to get a camera that you can pass into `-setCamera:` that fits the given coordinate bounds. ([#4790](https://github.com/mapbox/mapbox-gl-native/pull/4790))

### Annotations

- MGLPointAnnotation and custom MGLAnnotation implementations (but not MGLMultiPoint) can be backed by an MGLAnnotationView instead of an MGLAnnotationImage. MGLAnnotationView is a subclass of UIView, so you can use Core Animation and other familiar technologies with it. To associate an MGLAnnotation with an MGLAnnotationView, implement `-mapView:viewForAnnotation:` in your MGLMapViewDelegate class. ([#4801](https://github.com/mapbox/mapbox-gl-native/pull/4801))
- An MGLAnnotation can be relocated by changing its `coordinate` property in a KVO-compliant way. An MGLMultiPoint cannot be relocated. ([#3835](https://github.com/mapbox/mapbox-gl-native/pull/3835))
- Setting the `image` property of an MGLAnnotationImage to `nil` resets it to the default red pin image and reclaims resources that can be used to customize additional annotations. ([#3835](https://github.com/mapbox/mapbox-gl-native/pull/3835))
- An MGLPolygon can now have interior polygons, representing holes knocked out of the overall shape. ([#5110](https://github.com/mapbox/mapbox-gl-native/pull/5110))

### User location

- The user dot now moves smoothly between user location updates while user location tracking is disabled. ([#1582](https://github.com/mapbox/mapbox-gl-native/pull/1582))
- Fixed an issue preventing KVO change notifications from being generated on MGLMapView’s `userTrackingMode` key path when `-setUserTrackingMode:animated:` is called. ([#4724](https://github.com/mapbox/mapbox-gl-native/pull/4724))
- Fixed a crash setting MGLMapView’s `userLocationVerticalAlignment` property before a user location update has occurred. ([#5278](https://github.com/mapbox/mapbox-gl-native/pull/5278))
- Mapbox Telemetry is automatically disabled while the host application is running in the iOS Simulator. ([#4726](https://github.com/mapbox/mapbox-gl-native/pull/4726))

### Offline maps

- `MGLOfflinePackProgress` now indicates how many tiles have been downloaded and how much space they take up. ([#4874](https://github.com/mapbox/mapbox-gl-native/pull/4874))
- Fixed an issue where the tile cache could be included in iCloud backups on the first launch. ([#5124](https://github.com/mapbox/mapbox-gl-native/pull/5124), [#5601](https://github.com/mapbox/mapbox-gl-native/pull/5601))
- Suppressed “Unable to make space for entry” console spew. ([#4708](https://github.com/mapbox/mapbox-gl-native/pull/4708))
- Deprecated `-[MGLMapView emptyMemoryCache]`. ([#4725](https://github.com/mapbox/mapbox-gl-native/pull/4725))

### Packaging

- Improved the design of the generated API documentation. ([#5306](https://github.com/mapbox/mapbox-gl-native/pull/5306))
- Applications linking against the SDK static framework no longer need to add `-ObjC` to the Other Linker Flags (`OTHER_LDFLAGS`) build setting. If you previously added this flag solely for this SDK, removing the flag may potentially reduce the overall size of your application. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))
- Removed the `armv7s` slice from the SDK to reduce its size. iPhone 5 and iPhone 5c automatically use the `armv7` slice instead. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))
- The SDK is now localizable. No localizations are currently provided, other than English, but if you need a particular localization, you can install the SDK manually and drop a .lproj folder into the framework. ([#4783](https://github.com/mapbox/mapbox-gl-native/pull/4783))
- Removed unused SVG files from the SDK’s resource bundle. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))

### Other changes

- Added category methods on NSValue for converting to and from the structure types defined in MGLGeometry.h. ([#4802](https://github.com/mapbox/mapbox-gl-native/pull/4802))
- Added NSFormatter subclasses for converting geographic coordinates and directions into display strings. ([#4802](https://github.com/mapbox/mapbox-gl-native/pull/4802))
- Added `MGLCoordinateInCoordinateBounds()`, a function that tests whether or not a coordinate is in a given bounds. ([#5053](https://github.com/mapbox/mapbox-gl-native/pull/5053))

## 3.2.3 - June 7, 2016

- Fixed an issue preventing `-[MGLMapViewDelegate mapViewDidFinishLoadingMap:]` from being called when returning to the view controller containing the map view from another view controller. ([#5164](https://github.com/mapbox/mapbox-gl-native/pull/5164))
- Declarations in the API documentation are shown in both Objective-C and Swift. ([realm/jazzy#530](https://github.com/realm/jazzy/pull/530))

## 3.2.2 - May 10, 2016

- Existing MGLStyle class methods that return default style URLs have been deprecated in favor of new methods that require an explicit style version parameter. The deprecated, unversioned methods continue to return version 8 of the respective styles and will not be updated as new versions of the styles are released. ([#4759](https://github.com/mapbox/mapbox-gl-native/pull/4759))
- Deprecated `+[MGLStyle emeraldStyleURL]` with no replacement method. To use the Emerald style going forward, we recommend that you use the underlying URL. ([#4759](https://github.com/mapbox/mapbox-gl-native/pull/4759))
- Added `+[MGLStyle outdoorsStyleURLWithVersion:]` for the new Outdoors style. ([#4759](https://github.com/mapbox/mapbox-gl-native/pull/4759))
- The Hybrid style is now called Satellite Streets. ([#4759](https://github.com/mapbox/mapbox-gl-native/pull/4759))

## 3.2.1 - April 20, 2016

- Fixed a hang that could occur if the host application attempts to set user defaults on a background queue. ([#4745](https://github.com/mapbox/mapbox-gl-native/pull/4745))
- User location heading updates now resume properly when an app becomes active again. ([#4674](https://github.com/mapbox/mapbox-gl-native/pull/4674))
- Fixed an issue causing hyperlinks in the documentation to be displayed as raw Markdown syntax when viewed in Xcode’s Quick Help popover or sidebar. ([#4760](https://github.com/mapbox/mapbox-gl-native/pull/4760))

## 3.2.0 - April 5, 2016

- If you’ve previously installed the SDK as a static framework, the installation workflow has changed to address issues when submitting your application to the App Store or installing it on a device. Upon upgrading to this version of the SDK, you’ll need to add Mapbox.bundle to the Copy Bundle Resources build phase and remove Mapbox.framework from the Embed Frameworks build phase. ([#4455](https://github.com/mapbox/mapbox-gl-native/pull/4455))
- Offline packs can now be downloaded to allow users to view specific regions of the map offline. A new MGLOfflineStorage class provides APIs for managing MGLOfflinePacks. ([#4221](https://github.com/mapbox/mapbox-gl-native/pull/4221))
- Tiles and other resources are cached in the same file that holds offline resources. The combined cache file is located in a subdirectory of the user’s Application Support directory, which means iOS will not delete the file when disk space runs low. ([#4377](https://github.com/mapbox/mapbox-gl-native/pull/4377))
- Fixed an issue where the map view’s center would always be calculated as if the view occupied the entire screen. ([#4504](https://github.com/mapbox/mapbox-gl-native/issues/4504))
- The user dot no longer disappears after panning the map across the antimeridian at low zoom levels. ([#4275](https://github.com/mapbox/mapbox-gl-native/pull/4275))
- The map no longer recoils when panning quickly at low zoom levels. ([#4214](https://github.com/mapbox/mapbox-gl-native/pull/4214))
- Fixed an issue causing the map to pan the wrong way when the user pinches unevenly. ([#4427](https://github.com/mapbox/mapbox-gl-native/pull/4427))
- The maximum zoom level is capped at 21 due to rendering issues at higher zoom levels. ([#4417](https://github.com/mapbox/mapbox-gl-native/pull/4417))
- An icon laid out along a line no longer appears if it would extend past the end of the line. Some one-way arrows no longer point the wrong way. ([#3839](https://github.com/mapbox/mapbox-gl-native/pull/3839))
- Fixed warping of dashed lines near sharp corners. ([#3914](https://github.com/mapbox/mapbox-gl-native/pull/3914))
- Telemetry location gathering now occurs only when the device is in motion. ([#4115](https://github.com/mapbox/mapbox-gl-native/pull/4115))
- An account’s monthly active users metric only counts a user once a map view is displayed to that user. ([#3713](https://github.com/mapbox/mapbox-gl-native/pull/3713))
- A .dSYM bundle is included with the dynamic framework to facilitate symbolication of crash logs.
- Updated documentation to reflect the requirement that you must embed the dynamic framework in the “Embedded Binaries” section in Xcode. ([#4011](https://github.com/mapbox/mapbox-gl-native/issues/4011))
- Polygons and polylines now default to using the map view’s tint color. ([#4028](https://github.com/mapbox/mapbox-gl-native/pull/4028))
- The user dot no longer lags when panning the map. ([#3683](https://github.com/mapbox/mapbox-gl-native/pull/3683))
- The Improve This Map tool now uses the same zoom level that is currently being shown in the map view. ([#4068](https://github.com/mapbox/mapbox-gl-native/pull/4068))
- Fixed a formatting issue in the documentation for `MGLCoordinateBoundsIsEmpty()`. ([#3958](https://github.com/mapbox/mapbox-gl-native/pull/3958))

## 3.1.2 - February 22, 2016

- You can once again install the static framework without manually linking several framework and library dependencies. ([#4029](https://github.com/mapbox/mapbox-gl-native/pull/4029))
- The location manager used by MGLMapView to show the user’s location is now paused when the application is sent to the background. ([#4034](https://github.com/mapbox/mapbox-gl-native/pull/4034))

## 3.1.1 - February 15, 2016

- Corrected the dynamic framework’s minimum deployment target to iOS 8.0. ([#3872](https://github.com/mapbox/mapbox-gl-native/pull/3872))
- Fixed Fabric compatibility. ([#3847](https://github.com/mapbox/mapbox-gl-native/pull/3847))
- Fixed a crash that can occur when reselecting an annotation. ([#3881](https://github.com/mapbox/mapbox-gl-native/pull/3881))
- Fixed an issue preventing the Latitude inspectable from working when it is set before setting the Zoom Level inspectable in Interface Builder. ([#3886](https://github.com/mapbox/mapbox-gl-native/pull/3886))
- Fixed an issue that incorrectly expanded the tappable area of an annotation and prevented the annotation’s alignment rect insets from having any effect on the tappable area. ([#3898](https://github.com/mapbox/mapbox-gl-native/pull/3898))
- Fixed an issue preventing `-[MGLMapViewDelegate mapView:tapOnCalloutForAnnotation:]` from being called when a non-custom callout view is tapped. ([#3875](https://github.com/mapbox/mapbox-gl-native/pull/3875))

## 3.1.0 - February 5, 2016

- The SDK is now distributed as a dynamic framework instead of a static library, resulting in a simpler installation workflow and significantly reduced download size. The framework contains both simulator and device content. If you install the dynamic framework manually, you’ll need to strip out the simulator content before submitting your application to the App Store due to [an Xcode bug](http://www.openradar.me/radar?id=6409498411401216); see the installation instructions included with the framework for details. ([#3183](https://github.com/mapbox/mapbox-gl-native/pull/3183))
- Fixed an issue causing the entire MGLMapView to leak. ([#3448](https://github.com/mapbox/mapbox-gl-native/pull/3448))
- `MGLMapView` methods that alter the viewport now accept optional completion handlers. ([#3090](https://github.com/mapbox/mapbox-gl-native/pull/3090))
- You can now modify an annotation’s image after adding the annotation to the map. ([#3146](https://github.com/mapbox/mapbox-gl-native/pull/3146))
- Tapping now selects annotations more reliably. Tapping near the top of a large annotation image now selects that annotation. An annotation image’s alignment insets influence how far away the user can tap and still select the annotation. For example, if your annotation image has a large shadow, you can keep that shadow from being tappable by excluding it from the image’s alignment rect. ([#3261](https://github.com/mapbox/mapbox-gl-native/pull/3261))
- Annotations remain visible after switching to a different style. ([#3049](https://github.com/mapbox/mapbox-gl-native/pull/3049))
- The minimum and maximum zoom levels can now be configured using the `minimumZoomLevel` and `maximumZoomLevel` properties, respectively. The map is no longer limited to zoom level 18: by default, the maximum zoom level is now 20, allowing for a more detailed map in urban areas. ([#3712](https://github.com/mapbox/mapbox-gl-native/pull/3712))
- A new method on MGLMapView, `-flyToCamera:withDuration:completionHandler:`, lets you transition between viewpoints along an arc as if by aircraft. ([#3171](https://github.com/mapbox/mapbox-gl-native/pull/3171), [#3301](https://github.com/mapbox/mapbox-gl-native/pull/3301))
- MGLMapCamera’s `altitude` values now match those of MKMapCamera. ([#3362](https://github.com/mapbox/mapbox-gl-native/pull/3362))
- MGLMapView properties like `centerCoordinate` and `camera` now offset the center to account for any translucent top or bottom bar. As a result, when user tracking is enabled and the map view is an immediate child of a view controller, the user dot is centered in the unobscured portion of the map view. To override this offset, modify the `contentInset` property; you may also need to set the containing view controller’s `automaticallyAdjustsScrollViewInsets` property to `NO`. ([#3583](https://github.com/mapbox/mapbox-gl-native/pull/3583))
- In user tracking mode, the user dot stays in a fixed position within MGLMapView while the map pans smoothly. A new property, `userLocationVerticalAlignment`, determines the user dot’s fixed position. ([#3589](https://github.com/mapbox/mapbox-gl-native/pull/3589))
- When the user tracking mode is set to `MGLUserTrackingModeFollowWithCourse`, an optional `targetCoordinate` is kept within sight at all times as the user changes location. This property, in conjunction with the `userLocationVerticalAlignment` property, may be useful for displaying the user’s progress toward a waypoint. ([#3680](https://github.com/mapbox/mapbox-gl-native/pull/3680))
- Heading or course tracking mode can now be enabled as soon as an MGLMapView is initialized. ([#3680](https://github.com/mapbox/mapbox-gl-native/pull/3680))
- Zooming and rotation gestures no longer disable user tracking mode. ([#3589](https://github.com/mapbox/mapbox-gl-native/pull/3589))
- User tracking mode starts out at a lower zoom level by default. ([#3589](https://github.com/mapbox/mapbox-gl-native/pull/3589))
- Fixed an issue with small map views not properly fitting annotations within bounds. ([#3407](https://github.com/mapbox/mapbox-gl-native/pull/3407))
- When the user rotates the map to within 7° of true north, the map view now snaps to true north. ([#3403](https://github.com/mapbox/mapbox-gl-native/pull/3403))
- The map view’s background can now be transparent or translucent, as long as the style’s background layer is transparent or translucent and `MGLMapView.opaque` is set to `NO`. ([#3096](https://github.com/mapbox/mapbox-gl-native/pull/3096))
- Documentation is now generated by [jazzy](https://github.com/realm/jazzy) instead of appledoc. ♪♫ ([#3203](https://github.com/mapbox/mapbox-gl-native/pull/3203))
- New API to provide a custom callout view to the map for annotations. ([#3456](https://github.com/mapbox/mapbox-gl-native/pull/3456))
- Made telemetry on/off setting available in-app. ([#3445](https://github.com/mapbox/mapbox-gl-native/pull/3445))
- Fixed an issue with users not being counted by Mapbox if they had disabled telemetry. ([#3495](https://github.com/mapbox/mapbox-gl-native/pull/3495))
- Fixed crash caused by MGLAnnotationImage with non-integer width or height ([#2198](https://github.com/mapbox/mapbox-gl-native/issues/2198))
- Fixed “include of non-modular header” errors in Swift projects managed by CocoaPods. ([#3679](https://github.com/mapbox/mapbox-gl-native/pull/3679))
- Avoids triggering the blue background location status bar when user has granted "when in use" permission. ([#3671](https://github.com/mapbox/mapbox-gl-native/issues/3671))
- Deprecated the `debugActive` property and `-toggleDebug` method on MGLMapView in favor of a new `debugMask` property that exposes individual style debugging options. ([#3742](https://github.com/mapbox/mapbox-gl-native/pull/3742))

## 3.0.1 - December 7, 2015

- Fixed CoreTelephony.framework crash. ([#3170](https://github.com/mapbox/mapbox-gl-native/pull/3170))
- Fixed an issue preventing the compass from responding to taps after the compass is moved programmatically. ([#3117](https://github.com/mapbox/mapbox-gl-native/pull/3117))
- CocoaPods is now distributed via a (static) framework. ([#3181](https://github.com/mapbox/mapbox-gl-native/issues/3181))

## 3.0.0 - November 23, 2015

- If you install this SDK via CocoaPods, CocoaPods version 0.38.0 or above is required. ([#2132](https://github.com/mapbox/mapbox-gl-native/pull/2132))
- The `styleID` property has been removed from MGLMapView. Instead, set the `styleURL` property to an NSURL in the form `mapbox://styles/STYLE_ID`. If you previously set the style ID in Interface Builder’s Attributes inspector, delete the `styleID` entry from the User Defined Runtime Attributes section of the Identity inspector, then set the new “Style URL” inspectable to a value in the form `mapbox://styles/STYLE_ID`. ([#2632](https://github.com/mapbox/mapbox-gl-native/pull/2632))
- Default styles such as Streets are no longer bundled with the SDK; instead, they are loaded at runtime from the style API on mapbox.com. As always, you can use these default styles with any valid access token, and Streets continues to be `MGLMapView`’s initial style. The `bundledStyleURLs` property on `MGLMapView` has been deprecated in favor of several class methods on `MGLStyle` that provide direct access to the default styles. ([#2746](https://github.com/mapbox/mapbox-gl-native/pull/2746))
- The SDK now builds with Bitcode enabled. A version of libMapbox.a with Bitcode disabled is also available. ([#2332](https://github.com/mapbox/mapbox-gl-native/issues/2332), [#3003](https://github.com/mapbox/mapbox-gl-native/pull/3003))
- The style URL can be set to a local resource: `asset://local-color.json` and `local-color.json` both resolve to a file named `local-color.json` in the application’s root folder. ([#3087](https://github.com/mapbox/mapbox-gl-native/pull/3087))
- The double-tap-drag gesture for zooming in and out is now consistent with the Google Maps SDK. ([#2153](https://github.com/mapbox/mapbox-gl-native/pull/2153))
- A new `MGLAnnotationImage.enabled` property allows you to disable touch events on individual annotations. ([#2501](https://github.com/mapbox/mapbox-gl-native/pull/2501))
- Fixed a rendering issue that caused one-way arrows along tile boundaries to point due east instead of in the direction of travel. ([#2530](https://github.com/mapbox/mapbox-gl-native/pull/2530))
- Fixed an issue that prevented zoom level–dependent style properties from updating after zooming programmatically with animation. ([#2951](https://github.com/mapbox/mapbox-gl-native/pull/2951))
- Performance and appearance improvements during annotation adds & removes. ([#1688](https://github.com/mapbox/mapbox-gl-native/issues/1688))
- Overall improved performance during renders by not rendering faster than necessary. ([#1975](https://github.com/mapbox/mapbox-gl-native/issues/1975))
- Fixed a rendering issue with styles that use the `background-pattern` property. ([#2531](https://github.com/mapbox/mapbox-gl-native/pull/2531))
- Fixed a crash when reusing a single `MGLMapView` across multiple `UIViewController`s. ([#2969](https://github.com/mapbox/mapbox-gl-native/pull/2969))
- Fixed a crash on iPod touch and other devices or simulators without a cell carrier. ([#2687](https://github.com/mapbox/mapbox-gl-native/issues/2687))
- Eliminated flickering when opening and closing an overlay, such as an alert or action sheet. ([#2309](https://github.com/mapbox/mapbox-gl-native/pull/2309))
- Labels can now line wrap on hyphens and other punctuation. ([#2598](https://github.com/mapbox/mapbox-gl-native/pull/2598))
- A new delegate callback was added for observing taps to annotation callout views. ([#2596](https://github.com/mapbox/mapbox-gl-native/pull/2596))
- `-mapViewRegionIsChanging:` is now sent to the map view’s delegate during gestures. ([#2700](https://github.com/mapbox/mapbox-gl-native/pull/2700))
- Improved gesture recognition while the map is tilted. ([#2770](https://github.com/mapbox/mapbox-gl-native/pull/2770))
- `-mapViewWillStartLoadingMap:` and `-mapViewDidFinishLoadingMap:` delegate methods now work. ([#2706](https://github.com/mapbox/mapbox-gl-native/pull/2706))
- Removed CoreTelephony.framework dependency. ([#2581](https://github.com/mapbox/mapbox-gl-native/pull/2581))
- Improved user location annotation responsiveness. ([#2643](https://github.com/mapbox/mapbox-gl-native/pull/2643))

## 2.1.2 - September 15, 2015

- Built with Xcode 6.4 to not yet trigger Bitcode compatibility until Xcode 7 stabilizes. ([#2332](https://github.com/mapbox/mapbox-gl-native/issues/2332))

## 2.1.1 - September 15, 2015

- Fixes for Xcode 7 and Bitcode. ([#2238](https://github.com/mapbox/mapbox-gl-native/pull/2238))

## 2.1.0 - September 14, 2015

- A two-finger vertical swipe now tilts the map into perspective mode. ([#2116](https://github.com/mapbox/mapbox-gl-native/pull/2116))
- A new `MGLMapCamera` API allows you to transition multiple viewpoint properties, including rotation and pitch, simultaneously with an optional custom duration and timing function. ([#2193](https://github.com/mapbox/mapbox-gl-native/pull/2193))
- A new user tracking mode, `MGLUserTrackingModeFollowWithCourse`, has been added for indicating the current direction of travel. ([#2068](https://github.com/mapbox/mapbox-gl-native/pull/2068))
- Version 8 (`v8`) of the [Mapbox GL style spec](https://www.mapbox.com/mapbox-gl-style-spec/) is now required. If you are using a custom `v7` style, it needs to be upgraded using [this migrator script](https://github.com/mapbox/mapbox-gl-style-spec/blob/mb-pages/migrations/v7.js). ([#2052](https://github.com/mapbox/mapbox-gl-native/pull/2052))
- Applications built with Mapbox GL no longer crash when Location Services launches them in background mode. ([#1821](https://github.com/mapbox/mapbox-gl-native/pull/1821), [#1869](https://github.com/mapbox/mapbox-gl-native/pull/1869))
- Fixed a crash when adding annotations to an `MGLMapView` inside `-viewDidLoad`. ([#1874](https://github.com/mapbox/mapbox-gl-native/pull/1874))
- The user location annotation view now indicates the location reading’s accuracy and the device’s heading. ([#2010](https://github.com/mapbox/mapbox-gl-native/pull/2010))
- Eliminated linker warnings and errors when building against the iOS 9.0 SDK in Xcode 7. ([#1962](https://github.com/mapbox/mapbox-gl-native/pull/1962))
- Worked around a bug in the iOS 9.0 SDK that caused a crash on launch. ([#1958](https://github.com/mapbox/mapbox-gl-native/pull/1958))
- User location tracking no longer sends `MGLMapView` into an invalid region on iOS 9. ([#1925](https://github.com/mapbox/mapbox-gl-native/pull/1925))
- Eliminated console spew in the iOS demo application that was related to Mapbox Metrics HTTP requests. ([#1937](https://github.com/mapbox/mapbox-gl-native/issues/1937))
- Implemented `-[MGLMapView showAnnotations:animated:]`. ([#2050](https://github.com/mapbox/mapbox-gl-native/pull/2050))
- Fixed a crash adding a shape annotation with zero points. ([#2098](https://github.com/mapbox/mapbox-gl-native/pull/2098))
- Debug mode now displays information useful for debugging the label collision algorithm. ([#1808](https://github.com/mapbox/mapbox-gl-native/pull/1808))
- Minor style updates. ([#1910](https://github.com/mapbox/mapbox-gl-native/pull/1910))
- The CocoaPods pod now contains a `README.md` file. ([#1886](https://github.com/mapbox/mapbox-gl-native/pull/1886))

## 2.0.0 - August 21, 2015

Repackaging 2.0.0-pre.1 as it contained no issues.

## 2.0.0-pre.1 - August 21, 2015

Repackaging 0.5.1 as the Mapbox iOS SDK 2.0.0 series.

## 0.5.1 - July 13, 2015

### iOS

- Added support for CocoaPods 0.38.0. ([#1876](https://github.com/mapbox/mapbox-gl-native/pull/1876))

## 0.5.0 - July 9, 2015

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

## 0.4.0 - June 19, 2015

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

## 0.3.1 - May 15, 2015

- Temporarily removed `IBDesignable` support on iOS.

## 0.3.0 - May 14, 2015

- Initial iOS beta release.

Known issues:

- None.
