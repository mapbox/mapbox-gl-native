# Changelog for Mapbox iOS SDK

Mapbox welcomes participation and contributions from everyone.  If you’d like to do so please see [CONTRIBUTING.md](https://github.com/mapbox/mapbox-gl-native/blob/master/CONTRIBUTING.md) first to get started.

## master

- Applications linking against the SDK static framework no longer need to add `-ObjC` to the Other Linker Flags (`OTHER_LDFLAGS`) build setting. If you previously added this flag solely for this SDK, removing the flag may potentially reduce the overall size of your application. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))
- Removed the `armv7s` slice from the SDK to reduce its size. iPhone 5 and iPhone 5c automatically use the `armv7` slice instead. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))
- The user dot now moves smoothly between user location updates while user location tracking is disabled. ([#1582](https://github.com/mapbox/mapbox-gl-native/pull/1582))
- User location heading updates now resume properly when an app becomes active again. ([#4674](https://github.com/mapbox/mapbox-gl-native/pull/4674))
- Fixed an issue preventing KVO change notifications from being generated on MGLMapView’s `userTrackingMode` key path when `-setUserTrackingMode:animated:` is called. ([#4724](https://github.com/mapbox/mapbox-gl-native/pull/4724))
- Fixed a hang that could occur if the host application attempts to set user defaults on a background queue. ([#4745](https://github.com/mapbox/mapbox-gl-native/pull/4745))
- Added a `-reloadStyle:` action to MGLMapView to force a reload of the current style. ([#4728](https://github.com/mapbox/mapbox-gl-native/pull/4728))
- A more specific user agent string is now sent with style and tile requests. ([#4012](https://github.com/mapbox/mapbox-gl-native/pull/4012))
- Mapbox Telemetry is automatically disabled while the host application is running in the iOS Simulator. ([#4726](https://github.com/mapbox/mapbox-gl-native/pull/4726))
- Removed unused SVG files from the SDK’s resource bundle. ([#4641](https://github.com/mapbox/mapbox-gl-native/pull/4641))
- Deprecated `-[MGLMapView emptyMemoryCache]`. ([#4725](https://github.com/mapbox/mapbox-gl-native/pull/4725))

## 3.2.0

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

## 3.1.2

- You can once again install the static framework without manually linking several framework and library dependencies. ([#4029](https://github.com/mapbox/mapbox-gl-native/pull/4029))
- The location manager used by MGLMapView to show the user’s location is now paused when the application is sent to the background. ([#4034](https://github.com/mapbox/mapbox-gl-native/pull/4034))

## 3.1.1

- Corrected the dynamic framework’s minimum deployment target to iOS 8.0. ([#3872](https://github.com/mapbox/mapbox-gl-native/pull/3872))
- Fixed Fabric compatibility. ([#3847](https://github.com/mapbox/mapbox-gl-native/pull/3847))
- Fixed a crash that can occur when reselecting an annotation. ([#3881](https://github.com/mapbox/mapbox-gl-native/pull/3881))
- Fixed an issue preventing the Latitude inspectable from working when it is set before setting the Zoom Level inspectable in Interface Builder. ([#3886](https://github.com/mapbox/mapbox-gl-native/pull/3886))
- Fixed an issue that incorrectly expanded the tappable area of an annotation and prevented the annotation’s alignment rect insets from having any effect on the tappable area. ([#3898](https://github.com/mapbox/mapbox-gl-native/pull/3898))
- Fixed an issue preventing `-[MGLMapViewDelegate mapView:tapOnCalloutForAnnotation:]` from being called when a non-custom callout view is tapped. ([#3875](https://github.com/mapbox/mapbox-gl-native/pull/3875))

## 3.1.0

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
- Fixed an issue with small map views not properly fitting annotations within bounds. (#[3407](https://github.com/mapbox/mapbox-gl-native/pull/3407))
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

## 3.0.1

- Fixed CoreTelephony.framework crash. ([#3170](https://github.com/mapbox/mapbox-gl-native/pull/3170))
- Fixed an issue preventing the compass from responding to taps after the compass is moved programmatically. ([#3117](https://github.com/mapbox/mapbox-gl-native/pull/3117))
- CocoaPods is now distributed via a (static) framework. ([#3181](https://github.com/mapbox/mapbox-gl-native/issues/3181))

## 3.0.0

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

## 2.1.2

- Built with Xcode 6.4 to not yet trigger Bitcode compatibility until Xcode 7 stabilizes. ([#2332](https://github.com/mapbox/mapbox-gl-native/issues/2332))

## 2.1.1

- Fixes for Xcode 7 and Bitcode. ([#2238](https://github.com/mapbox/mapbox-gl-native/pull/2238))

## 2.1.0

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

## 2.0.0

Repackaging 2.0.0-pre.1 as it contained no issues.

## 2.0.0-pre.1

Repackaging 0.5.1 as the Mapbox iOS SDK 2.0.0 series.

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
