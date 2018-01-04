# Changelog for Mapbox Maps SDK for macOS

## master

* Renamed this SDK from Mapbox macOS SDK to Mapbox Maps SDK for macOS. ([#10610](https://github.com/mapbox/mapbox-gl-native/pull/10610))

### Annotations and user interaction

* Increased the default maximum zoom level from 20 to 22. ([#9835](https://github.com/mapbox/mapbox-gl-native/pull/9835))

### Styles

* Fixed an issue preventing a dynamically-added `MGLRasterStyleLayer` from drawing until the map pans. ([#10270](https://github.com/mapbox/mapbox-gl-native/pull/10270))
* Added `MGLComputedShapeSource` source class that allows applications to supply vector data on a per-tile basis.

## v0.6.0

### Packaging

* Renamed this SDK from Mapbox macOS SDK to Mapbox Maps SDK for macOS. ([#10610](https://github.com/mapbox/mapbox-gl-native/pull/10610), [#10793](https://github.com/mapbox/mapbox-gl-native/pull/10793))
* Added a Bulgarian localization. ([#10309](https://github.com/mapbox/mapbox-gl-native/pull/10309))

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
* Improved the legibility of labels that follow lines when the map is tilted. ([#9009](https://github.com/mapbox/mapbox-gl-native/pull/9009))
* Fixed an issue that could cause flickering when a translucent raster style layer was present. ([#9468](https://github.com/mapbox/mapbox-gl-native/pull/9468))
* Fixed an issue that could cause antialiasing between polygons on the same layer to fail if the fill layers used data-driven styling for the fill color. ([#9699](https://github.com/mapbox/mapbox-gl-native/pull/9699))
* The previously deprecated support for style classes has been removed. For interface compatibility, the API methods remain, but they are now non-functional.

### Annotations and user interaction

* Fixed several bugs and performance issues related to the use of annotations backed by `MGLAnnotationImage`s. The limits on the number and size of images and glyphs has been effectively eliminated and should now depend on hardware constraints. These fixes also apply to images used to represent icons in `MGLSymbolStyleLayer`s. ([#9213](https://github.com/mapbox/mapbox-gl-native/pull/9213))
* Increased the default maximum zoom level from 20 to 22. ([#9835](https://github.com/mapbox/mapbox-gl-native/pull/9835))
* Added an `overlays` property to `MGLMapView`. ([#8617](https://github.com/mapbox/mapbox-gl-native/pull/8617))
* Fixed incorrect hit targets for `MGLAnnotationImage`-backed annotations that caused `-[MGLMapViewDelegate mapView:didSelectAnnotation:]` to be called unnecessarily. ([#10538](https://github.com/mapbox/mapbox-gl-native/pull/10538))
* Added `-[MGLMapView cameraThatFitsShape:direction:edgePadding:]` to get a camera with zoom level and center coordinate computed to fit a shape. ([#10107](https://github.com/mapbox/mapbox-gl-native/pull/10107))
* Added support selection of shape and polyline annotations.([#9984](https://github.com/mapbox/mapbox-gl-native/pull/9984))
* Fixed an issue where a shape annotation callout was not displayed if the centroid was not visible. ([#10255](https://github.com/mapbox/mapbox-gl-native/pull/10255))

### Other changes

* Fixed distortion in the logo view on macOS 10.13 High Sierra. ([#10606](https://github.com/mapbox/mapbox-gl-native/pull/10606))
* Fixed an issue that could cause line label rendering glitches when the line geometry is projected to a point behind the plane of the camera. ([#9865](https://github.com/mapbox/mapbox-gl-native/pull/9865))
* Fixed an issue that could cause a crash when using `-[MGLMapView flyToCamera:completionHandler:]` and related methods with zoom levels at or near the maximum value. ([#9381](https://github.com/mapbox/mapbox-gl-native/pull/9381))
* Fixed an issue where removing a `MGLOpenGLStyleLayer` from a map might result in a crash. ([#10765](https://github.com/mapbox/mapbox-gl-native/pull/10765))
* Added documentation for usage of coordinate bounds that cross the anti-meridian. ([#10783](https://github.com/mapbox/mapbox-gl-native/pull/10783))
* Removed duplicated variables in `MGLMapSnapshotter`. ([#10702](https://github.com/mapbox/mapbox-gl-native/pull/10702))

## 0.5.1

This version of the Mapbox macOS SDK corresponds to version 3.6.4 of the Mapbox iOS SDK.

* Added an `MGLStyle.localizesLabels` property, off by default, that localizes any Mapbox Streets–sourced symbol layer into the user’s preferred language. ([#9582](https://github.com/mapbox/mapbox-gl-native/pull/9582))
* Fixed an issue that caused `-[MGLShapeSource featuresMatchingPredicate:]` and `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` to always return an empty array. ([#9784](https://github.com/mapbox/mapbox-gl-native/pull/9784))
* `MGLMapView`’s `minimumZoomLevel` and `maximumZoomLevel` properties are now available in Interface Builder’s Attributes inspector. ([#9729](https://github.com/mapbox/mapbox-gl-native/pull/9729))
* Added a Hungarian localization. ([#9945](https://github.com/mapbox/mapbox-gl-native/pull/9945))
* Deprecated `+[MGLStyle trafficDayStyleURL]` and `+[MGLStyle trafficNightStyleURL]` with no replacement method. To use the Traffic Day and Traffic Night styles going forward, we recommend that you use the underlying URL. ([#9918](https://github.com/mapbox/mapbox-gl-native/pull/9918))
* Fixed an issue where stale (but still valid) map data could be ignored in offline mode. ([#10012](https://github.com/mapbox/mapbox-gl-native/pull/10012))

## 0.5.0

This version of the Mapbox macOS SDK corresponds to version 3.6.0 of the Mapbox iOS SDK.

### Packaging

* Xcode 8.0 or higher is now recommended for using this SDK. ([#8775](https://github.com/mapbox/mapbox-gl-native/pull/8775))
* Updated MGLMapView’s logo view to display [the new Mapbox logo](https://www.mapbox.com/blog/new-mapbox-logo/). ([#8771](https://github.com/mapbox/mapbox-gl-native/pull/8771), [#8773](https://github.com/mapbox/mapbox-gl-native/pull/8773))

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

* The default marker image has been made slightly larger and now matches the version in the Mapbox iOS SDK. ([#9370](https://github.com/mapbox/mapbox-gl-native/pull/9370))
* The `MGLPolyline.coordinate` and `MGLPolygon.coordinate` properties now return the midpoint and centroid, respectively, instead of the first coordinate. ([#8713](https://github.com/mapbox/mapbox-gl-native/pull/8713))

### User interaction

* Fixed an issue causing the map to go blank during a flight animation that travels a very short distance. ([#9199](https://github.com/mapbox/mapbox-gl-native/pull/9199))
* Fixed an issue causing the mouse cursor to jump after Shift- or Option-dragging the map. ([#9390](https://github.com/mapbox/mapbox-gl-native/pull/9390))
* The Improve This Map button in the attribution action sheet now leads to a feedback tool that matches MGLMapView’s rotation and pitch. `-[MGLAttributionInfo feedbackURLAtCenterCoordinate:zoomLevel:]` no longer respects the feedback URL specified in TileJSON. ([#9078](https://github.com/mapbox/mapbox-gl-native/pull/9078))

### Other changes

* Fixed a crash when calling `MGLMultiPolygon.coordinate` [#8713](https://github.com/mapbox/mapbox-gl-native/pull/8713)
* Fixed an issue causing attribution button text to appear blue instead of black. ([#8701](https://github.com/mapbox/mapbox-gl-native/pull/8701))
* Fixed a crash or console spew when MGLMapView is initialized with a frame smaller than 64 points wide by 64 points tall. ([#8562](https://github.com/mapbox/mapbox-gl-native/pull/8562))
* The error passed into `-[MGLMapViewDelegate mapViewDidFailLoadingMap:withError:]` now includes a more specific description and failure reason. ([#8418](https://github.com/mapbox/mapbox-gl-native/pull/8418))
* Improved CPU and battery performance while animating a tilted map’s camera in an area with many labels. ([#9031](https://github.com/mapbox/mapbox-gl-native/pull/9031))
* Fixed an issue rendering polylines that contain duplicate vertices. ([#8808](https://github.com/mapbox/mapbox-gl-native/pull/8808))
* Added struct boxing to `MGLCoordinateSpan`, `MGLCoordinateBounds`, `MGLOfflinePackProgress`, and `MGLTransition`. ([#9343](https://github.com/mapbox/mapbox-gl-native/pull/9343))

## 0.4.1 - April 8, 2017

This version of the Mapbox macOS SDK corresponds to version 3.5.2 of the Mapbox iOS SDK.

* Fixed an issue causing code signing failures and bloating the framework. ([#8640](https://github.com/mapbox/mapbox-gl-native/pull/8640))
* Fixed an issue that could cause a crash if annotations unknown to the map view were interacted with. ([#8686](https://github.com/mapbox/mapbox-gl-native/pull/8686))
* Renamed the “Data-Driven Styling” guide to “Using Style Functions at Runtime” and clarified the meaning of data-driven styling in the guide’s discussion of runtime style functions. ([#8627](https://github.com/mapbox/mapbox-gl-native/pull/8627))

## 0.4.0 - April 2, 2017

This version of the Mapbox macOS SDK corresponds to version 3.5.1 of the Mapbox iOS SDK.

### Internationalization

* Added support for right-to-left text and Arabic ligatures in labels. ([#6984](https://github.com/mapbox/mapbox-gl-native/pull/6984), [#7123](https://github.com/mapbox/mapbox-gl-native/pull/7123))
* Improved the line wrapping behavior of point-placed labels, especially labels written in Chinese and Japanese. ([#6828](https://github.com/mapbox/mapbox-gl-native/pull/6828), [#7446](https://github.com/mapbox/mapbox-gl-native/pull/7446))
* CJK characters now remain upright in vertically oriented labels that have line placement, such as road labels. ([#7114](https://github.com/mapbox/mapbox-gl-native/issues/7114))
* Added Catalan, Chinese (Simplified and Traditional), Dutch, Finnish, French, German, Japanese, Lithuanian, Polish, Portuguese (Brazilian), Spanish, Swedish, Ukrainian, and Vietnamese localizations. ([#7316](https://github.com/mapbox/mapbox-gl-native/pull/7316), [#7503](https://github.com/mapbox/mapbox-gl-native/pull/7503), [#7899](https://github.com/mapbox/mapbox-gl-native/pull/7899), [#7999](https://github.com/mapbox/mapbox-gl-native/pull/7999), [#8113](https://github.com/mapbox/mapbox-gl-native/pull/8113), [#8256](https://github.com/mapbox/mapbox-gl-native/pull/8256))

### Styles

* Added support for data-driven styling in the form of source and composite style functions. `MGLStyleFunction` is now an abstract class, with `MGLCameraStyleFunction` providing the behavior of `MGLStyleFunction` in previous releases. New `MGLStyleFunction` subclasses allow you to vary a style attribute by the values of attributes of features in the source. ([#7596](https://github.com/mapbox/mapbox-gl-native/pull/7596))
* Added methods to MGLShapeSource and MGLVectorSource for querying features loaded by the source, whether or not they’re visible on the map. ([#8263](https://github.com/mapbox/mapbox-gl-native/pull/8263))
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
* When setting an `MGLShapeSource`’s shape to an `MGLFeature` instance, any `NSColor` attribute value is now converted to the equivalent CSS string representation for use with `MGLInterpolationModeIdentity` in style functions. ([#8025](https://github.com/mapbox/mapbox-gl-native/pull/8025))
* An exception is no longer thrown if layers or sources are removed from a style before they are added. ([#7962](https://github.com/mapbox/mapbox-gl-native/pull/7962))
* Renamed MGLStyleConstantValue to MGLConstantStyleValue. For compatibility with previous releases, MGLStyleConstantValue is now an alias of MGLConstantStyleValue. ([#8090](https://github.com/mapbox/mapbox-gl-native/pull/8090))
* Fixed a crash that could occur when switching styles after adding an MGLSource to the style. ([#8298](https://github.com/mapbox/mapbox-gl-native/pull/8298))

### Annotations and user interaction

* Added a method to MGLMapViewDelegate, `-mapView:shouldChangeFromCamera:toCamera:`, that you can implement to restrict which parts the user can navigate to using gestures. ([#5584](https://github.com/mapbox/mapbox-gl-native/pull/5584))
* When a map view is the first responder, pressing <kbd>+</kbd>, <kbd>-</kbd>, or <kbd>=</kbd> now zooms the map. ([#8033](https://github.com/mapbox/mapbox-gl-native/pull/8033))
* Changing the coordinates of a point annotation no longer deselects the annotation. ([#8269](https://github.com/mapbox/mapbox-gl-native/pull/8269))
* Fixed an issue that could cause a crash when point annotations were added and removed while simultaneously querying source features. ([#8374](https://github.com/mapbox/mapbox-gl-native/pull/8374))
* Fixed an issue preventing MGLMapView from adding a polyline annotation with the same coordinates as a polygon annotation. ([#8355](https://github.com/mapbox/mapbox-gl-native/pull/8355))
* Zooming by double-tap, two-finger tap, zoom buttons, shortcut keys, or demo app menu items or shortcut keys now zooms to the nearest integer zoom level. ([#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027))
* Fixed an issue where translucent point annotations along tile boundaries would be drawn darker than expected. ([#6832](https://github.com/mapbox/mapbox-gl-native/pull/6832))

### Networking and offline maps

* Offline pack notifications are now posted by `MGLOfflinePack` instances instead of the shared `MGLOfflineStorage` object. For backwards compatibility, the `userInfo` dictionary still indicates the pack’s state and progress. ([#7952](https://github.com/mapbox/mapbox-gl-native/pull/7952))
* Fixed a memory leak in MGLMapView. ([#7956](https://github.com/mapbox/mapbox-gl-native/pull/7956))
* Fixed an issue that could prevent a cached style from appearing while the computer is offline. ([#7770](https://github.com/mapbox/mapbox-gl-native/pull/7770))
* Fixed an issue that could prevent a style from loading when reestablishing a network connection. ([#7902](https://github.com/mapbox/mapbox-gl-native/pull/7902))
* `MGLOfflineStorage` instances now support a delegate conforming to `MGLOfflineStorageDelegate`, which allows altering URLs before they are requested from the Internet. ([#8084](https://github.com/mapbox/mapbox-gl-native/pull/8084))

### Other changes

* Added support for the Carthage dependency manager. See [this SDK’s homepage](https://mapbox.github.io/mapbox-gl-native/macos/) for setup instructions. ([#8257](https://github.com/mapbox/mapbox-gl-native/pull/8257))
* Fixed an issue that, among other things, caused various islands to disappear at certain zoom levels. ([#7621](https://github.com/mapbox/mapbox-gl-native/pull/7621))
* Added a method to MGLMapView that allows you to specify a predicate when querying for visible features. ([#8256](https://github.com/mapbox/mapbox-gl-native/pull/8246))
* Fixed flickering that occurred when panning past the antimeridian. ([#7574](https://github.com/mapbox/mapbox-gl-native/pull/7574))
* Added a `MGLDistanceFormatter` class for formatting geographic distances. ([#7888](https://github.com/mapbox/mapbox-gl-native/pull/7888))

## 0.3.1 - February 21, 2017

This version of the Mapbox macOS SDK corresponds to version 3.4.2 of the Mapbox iOS SDK.

* Fixed an issue causing MGLMapView’s `camera`’s `heading` to be set to a negative value, indicating an undefined heading, when the map view faces northwest. The heading is now wrapped to between zero and 360 degrees, for consistency with MGLMapView’s `direction` property. ([#7724](https://github.com/mapbox/mapbox-gl-native/pull/7724))
* Fixed a crash that occurred when moving a window containing an MGLMapView from one screen to another. ([#8004](https://github.com/mapbox/mapbox-gl-native/pull/8004))
* Fixed an issue preventing the use of the integrated GPU on machines that have more than one GPU. Follow the instructions in [Technical Q&A 1734](https://developer.apple.com/library/content/qa/qa1734/_index.html) to enable integrated GPU usage in your application. ([#7834](https://github.com/mapbox/mapbox-gl-native/pull/7834))
* Fixed an issue causing the mouse cursor to jump after shift- or option-dragging a map view if the window opened on a screen with a different size than the screen with keyboard focus. ([#7846](https://github.com/mapbox/mapbox-gl-native/pull/7846))
* Deprecated the style class methods in MGLStyle. ([#7785](https://github.com/mapbox/mapbox-gl-native/pull/7785))
* Improved the performance of trivial camera animations. ([#7125](https://github.com/mapbox/mapbox-gl-native/pull/7125))

## 0.3.0 - January 21, 2016

This version of the Mapbox macOS SDK corresponds to version 3.4.0 of the Mapbox iOS SDK. The two SDKs have very similar feature sets. The main differences are the lack of user location tracking and annotation views. Some APIs have been adapted to macOS conventions, particularly the use of NSPopover for callout views.

### Packaging

* Fixed an issue causing code signing failures and bloating the framework. ([#5850](https://github.com/mapbox/mapbox-gl-native/pull/5850))
* Xcode 7.3 or higher is now required for using this SDK. ([#6059](https://github.com/mapbox/mapbox-gl-native/issues/6059))
* Fixed an issue with symbols not being properly stripped from the dynamic framework when built with `make xpackage SYMBOLS=NO`. ([#6531](https://github.com/mapbox/mapbox-gl-native/pull/6531))
* The API reference has a sharper look. ([#7422](https://github.com/mapbox/mapbox-gl-native/pull/7422))
* Added documentation for the Info.plist keys used by this SDK. ([#6833](https://github.com/mapbox/mapbox-gl-native/pull/6833))

### Styles and data

* A new runtime styling API allows you to adjust the style and content of the base map dynamically. All the options available in [Mapbox Studio](https://www.mapbox.com/studio/) are now exposed via MGLStyle and subclasses of MGLStyleLayer and MGLSource. ([#5727](https://github.com/mapbox/mapbox-gl-native/pull/5727))
* MGLMapView’s `styleURL` property can now be set to an absolute file URL. ([#6026](https://github.com/mapbox/mapbox-gl-native/pull/6026))
* When creating an MGLShapeSource, you can now specify options for clustering point features within the shape source. Similarly, GeoJSON sources specified by the stylesheet at design time can specify the `cluster`, `clusterMaxZoom`, and `clusterRadius` attributes. ([#5724](https://github.com/mapbox/mapbox-gl-native/pull/5724))
* When creating an MGLTileSource, you can now specify that the tile URLs use [TMS](https://en.wikipedia.org/wiki/Tile_Map_Service) coordinates by setting `MGLTileSourceOptionTileCoordinateSystem` to `MGLTileCoordinateSystemTMS`. TileJSON files can specify `"scheme": "tms"`. ([#2270](https://github.com/mapbox/mapbox-gl-native/pull/2270))
* Fixed an issue causing abstract `MGLMultiPointFeature` objects to be returned in feature query results. Now concrete `MGLPointCollectionFeature` objects are returned. MGLMultiPointFeature is now an alias of MGLPointCollectionFeature. ([#6742](https://github.com/mapbox/mapbox-gl-native/pull/6742))
* Fixed rendering artifacts and missing glyphs that occurred after viewing a large number of CJK characters on the map. ([#5908](https://github.com/mapbox/mapbox-gl-native/pull/5908))
* Fixed an issue where the style zoom levels were not respected when deciding when to render a layer. ([#5811](https://github.com/mapbox/mapbox-gl-native/issues/5811))
* Fixed an issue where feature querying sometimes failed to return the expected features when the map was tilted. ([#6773](https://github.com/mapbox/mapbox-gl-native/pull/6773))
* MGLFeature’s `attributes` and `identifier` properties are now writable. ([#6728](https://github.com/mapbox/mapbox-gl-native/pull/6728))
* Attribution views now display the correct attribution for the current style. ([#5999](https://github.com/mapbox/mapbox-gl-native/pull/5999))
* If MGLMapView is unable to obtain or parse a style, it now calls its delegate’s `-mapViewDidFailLoadingMap:withError:` method. ([#6145](https://github.com/mapbox/mapbox-gl-native/pull/6145))
* Added the `-[MGLMapViewDelegate mapView:didFinishLoadingStyle:]` delegate method, which offers the earliest opportunity to modify the layout or appearance of the current style before the map view is displayed to the user. ([#6636](https://github.com/mapbox/mapbox-gl-native/pull/6636))
* Fixed an issue causing stepwise zoom functions to be misinterpreted. ([#6328](https://github.com/mapbox/mapbox-gl-native/pull/6328))
* A source’s tiles are no longer rendered when the map is outside the source’s supported zoom levels. ([#6345](https://github.com/mapbox/mapbox-gl-native/pull/6345))
* Fixed crashes that could occur when loading a malformed stylesheet. ([#5736](https://github.com/mapbox/mapbox-gl-native/pull/5736))
* Improved style parsing performance. ([#6170](https://github.com/mapbox/mapbox-gl-native/pull/6170))
* Improved feature querying performance. ([#6514](https://github.com/mapbox/mapbox-gl-native/pull/6514))
* Fixed an issue where shapes that cannot currently be visually represented as annotations were still shown on the map as point annotations. ([#6764](https://github.com/mapbox/mapbox-gl-native/issues/6764))

### Annotations

* Added `showAnnotations:animated:` and `showAnnotations:edgePadding:animated:`, which moves the map viewport to show the specified annotations. ([#5749](https://github.com/mapbox/mapbox-gl-native/pull/5749))
* Added new methods to MGLMultiPoint for changing the vertices along a polyline annotation or the exterior of a polygon annotation. ([#6565](https://github.com/mapbox/mapbox-gl-native/pull/6565))
* Fixed an exception raised when adding a custom annotation model object to MGLMapView. ([#7746](https://github.com/mapbox/mapbox-gl-native/pull/7746))
* Added new APIs to MGLMapView to query for visible annotations. ([#6061](https://github.com/mapbox/mapbox-gl-native/pull/6061))
* Shape, feature, and annotation classes now conform to NSSecureCoding. ([#6559](https://github.com/mapbox/mapbox-gl-native/pull/6559))
* Various method arguments that are represented as C arrays of `CLLocationCoordinate2D` instances have been marked `const` to streamline bridging to Swift. ([#7215](https://github.com/mapbox/mapbox-gl-native/pull/7215))
* To make an MGLPolyline or MGLPolygon span the antimeridian, specify coordinates with longitudes greater than 180° or less than −180°. ([#6088](https://github.com/mapbox/mapbox-gl-native/pull/6088))
* Fixed an issue where placing a point annotation on Null Island also placed a duplicate annotation on its antipode. ([#3563](https://github.com/mapbox/mapbox-gl-native/pull/3563))
* Fixed an issue that caused an assertion failure if a `MGLShapeCollection` (a GeoJSON GeometryCollection) was created with an empty array of shapes. ([#7632](https://github.com/mapbox/mapbox-gl-native/pull/7632))
* Improved the precision of annotations at zoom levels greater than 18. ([#5517](https://github.com/mapbox/mapbox-gl-native/pull/5517))

### Networking and offline maps

* Fixed an issue preventing an MGLMapView from loading tiles while an offline pack is downloading. ([#6446](https://github.com/mapbox/mapbox-gl-native/pull/6446))
* Fixed an issue causing an MGLOfflinePack’s progress to continue to update after calling `-suspend`. ([#6186](https://github.com/mapbox/mapbox-gl-native/pull/6186))
* Fixed an issue preventing cached annotation images from displaying while the device is offline. ([#6358](https://github.com/mapbox/mapbox-gl-native/pull/6358))
* Fixed a crash that could occur when the device is disconnected while downloading an offline pack. ([#6293](https://github.com/mapbox/mapbox-gl-native/pull/6293))
* Fixed a crash that occurred when encountering a rate-limit error in response to a network request. ([#6223](https://github.com/mapbox/mapbox-gl-native/pull/6223))
* Added support for an `MGLMapboxAPIBaseURL` key in an app's `Info.plist` in order to customize the base URL used for retrieving map data, styles, and other resources. ([#6709](https://github.com/mapbox/mapbox-gl-native/pull/6709))
* Query parameters are no longer stripped from mapbox: URLs used as resource URLs. ([#6182](https://github.com/mapbox/mapbox-gl-native/pull/6182), [#6432](https://github.com/mapbox/mapbox-gl-native/pull/6432))
* Database errors are now logged to the console. ([#6291](https://github.com/mapbox/mapbox-gl-native/pull/6291))

### Other changes

* Raster tiles such as those from Mapbox Satellite are now cached, eliminating flashing while panning back and forth. ([#7091](https://github.com/mapbox/mapbox-gl-native/pull/7091))
* Fixed an issue where the map view’s center would always be calculated as if the view occupied the entire window. ([#6102](https://github.com/mapbox/mapbox-gl-native/pull/6102))
* Notification names and user info keys are now string enumeration values for ease of use in Swift. ([#6794](https://github.com/mapbox/mapbox-gl-native/pull/6794))
* Fixed a typo in the documentation for the MGLCompassDirectionFormatter class. ([#5879](https://github.com/mapbox/mapbox-gl-native/pull/5879))
* The NSClickGestureRecognizer on MGLMapView that is used for selecting annotations now fails if a click does not select an annotation. ([#7246](https://github.com/mapbox/mapbox-gl-native/pull/7246))

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
